//
//  Denoiser.cpp
//  Half
//
//  Created by junho on 2019. 7. 8..
//

#include <nori/denoiser.h>
#include <fstream>
#include <time.h>

NORI_NAMESPACE_BEGIN

void Denoiser::setColor(Bitmap *source) {
    input = source;
    result = new Bitmap(size);
}

void Denoiser::initialize(int imageWidht, int imageHeight, int sampleCount) {
    this->sampleCount = sampleCount;
    size = Vector2i(imageWidht, imageHeight);
    samples = new std::vector<std::vector<Color3f>*>();
    samples->reserve(imageWidht * imageHeight);
    for(int i=0;i<imageWidht * imageHeight;i++) {
        samples->push_back(new std::vector<Color3f>());
        samples->at(i)->reserve(sampleCount);
    }

    variances = new std::vector<float>();
    variances->reserve(imageWidht * imageHeight);
    for(int i=0;i<imageHeight * imageWidht;i++) {
        variances->push_back(0.f);
    }
    
    filterScales = new std::vector<int>();
    filterScales->reserve(imageWidht * imageHeight);
    for(int i=0;i<imageHeight * imageWidht;i++) {
        filterScales->push_back(0);
    }
    
    float lowSampleCount = 1-1.f/sampleCount;
    float gammaResult = -log(1-pow((1.9*gamma),1/sqrt(2)));
    biasFactor = lowSampleCount * gammaResult;
}

void Denoiser::calculate() {
    clock_t start, end;
    start = clock();
    //initailizing the filters
    filters = new std::vector<float*>();
    filterPdfs = new std::vector<float>();
    filters->reserve(DENOISER_FILTER_NUM);
    filterPdfs->reserve(DENOISER_FILTER_NUM);
    
    getVarianceFinal();
    
    for(int i=0;i<DENOISER_FILTER_NUM;i++) {
        float* temp = new float[DENOISER_FILTER_SIZE * DENOISER_FILTER_SIZE];
        float tempPdf = 0;
        float stddev = initialStddev * pow(stddevStep, i);
        for(int x = 0;x<DENOISER_FILTER_SIZE;x++) {
            for(int y=0;y<DENOISER_FILTER_SIZE;y++) {
                float value = eval(x - DENOISER_FILTER_SIZE/2, y - DENOISER_FILTER_SIZE/2, stddev, DENOISER_FILTER_SIZE/2);
                temp[x + y*DENOISER_FILTER_SIZE] = value;
                tempPdf += value;
                printf("%f ", value);
            }
            printf("\n");
        }
        printf("[filter %d] --standard deviation %f--\n", i + 1, stddev);
        filters->push_back(temp);
        filterPdfs->push_back(tempPdf);
    }
    
    std::cout<<"Start Adaptive Denoising... "<<endl;
    std::string filterPath = "denoiserData.ppm";
    std::ofstream out(filterPath);
    
    for(int y=0;y<size.y();y++) {
        for(int x=0;x<size.x();x++) {
            calculateMSE(x,y,0);
        }
    }
    
    filterSelection();
    
    out<< "P3\n" << size.x() << " " << size.y() << "\n255\n";
    for(int y=0;y<size.y();y++) {
        for(int x=0;x<size.x();x++) {
            //pushToFilter(x, y, 0);
            float returnVal = filterScales->at(x + y*size.x());
            int ir = int(255.99 * (1-(returnVal/DENOISER_FILTER_NUM)));
            out<< ir << " " << ir << " " << ir << "\n";
            pushToFilter(x, y);
        }
    }
    out.close();
    end = clock();
    printf("Done! Took %.3fsec\n",(float)(end-start)/CLOCKS_PER_SEC);
}

Color3f Denoiser::pushToFilter(int x, int y, int step) {
    if(x < DENOISER_FILTER_SIZE/2 || x >= size.x()-DENOISER_FILTER_SIZE/2) {
        result->coeffRef(y, x) = input->coeffRef(y, x);
        return input->coeffRef(y, x);
    }
    if(y < DENOISER_FILTER_SIZE/2 || y >= size.y()-DENOISER_FILTER_SIZE/2) {
        result->coeffRef(y, x) = input->coeffRef(y, x);
        return input->coeffRef(y, x);
    }
    
    result->coeffRef(y, x) = Color3f(0.f);
    
    if(step == -1)
        step = filterScales->at(x + y * size.x());
    
    
    for(int i=0;i<DENOISER_FILTER_SIZE;i++) {
        for(int j=0;j<DENOISER_FILTER_SIZE;j++) {
            result->coeffRef(y,x) += filters->at(step)[i + DENOISER_FILTER_SIZE * j]*input->coeffRef(y+j-DENOISER_FILTER_SIZE/2,x+i-DENOISER_FILTER_SIZE/2)/filterPdfs->at(step);
        }
    }
    
    return result->coeffRef(y, x);
}

int Denoiser::calculateMSE(int x, int y, int step) {
    if(step >= DENOISER_FILTER_NUM - 1) {
        filterScales->at(x + size.x() * y) = step;
        return step + 1;
    }
    if(x < DENOISER_FILTER_SIZE/2 || x >= size.x()-DENOISER_FILTER_SIZE/2) {
        filterScales->at(x + size.x() * y) = step;
        return 0;
    }
    if(y < DENOISER_FILTER_SIZE/2 || y >= size.y()-DENOISER_FILTER_SIZE/2) {
        filterScales->at(x + size.x() * y) = step;
        return 0;
    }
    
    int returnValue = step;
    
    float bias = 0.f;
    float variance = 0.f;
    float coarseScale = initialStddev * pow(stddevStep, step + 1);
    float fineScale = initialStddev * pow(stddevStep , step);
    float doubleCS = coarseScale * coarseScale;
    float doubleFS = fineScale * fineScale;
    Color3f fineResult = pushToFilter(x, y, step);
    Color3f coarseResult = pushToFilter(x, y, step + 1);
    float cminusf = pow(fineResult.x() - coarseResult.x(),2) +
                    pow(fineResult.y() - coarseResult.y(),2) +
                    pow(fineResult.z() - coarseResult.z(),2);
    
    bias = ((doubleCS + doubleFS) / (doubleCS - doubleFS)) * cminusf;
    bias *= biasFactor;
    
    for(int i=0;i<DENOISER_FILTER_SIZE;i++) {
        for(int j=0;j<DENOISER_FILTER_SIZE;j++) {
            int index = (y + j - DENOISER_FILTER_SIZE/2) * size.x() + x + i - DENOISER_FILTER_SIZE/2;
            variance += (filters->at(step + 1)[i + DENOISER_FILTER_SIZE * j] - filters->at(step)[i + DENOISER_FILTER_SIZE * j]) * variances->at(index);
        }
    }
    
    if(bias + variance <= 0) {
        returnValue = calculateMSE(x, y, step + 1);
    }
    else {
        if(step == 0) {
            filterScales->at(x + size.x() * y) = step;
            return 0;
        }
        filterScales->at(x + size.x() * y) = step;
    }
    
    return returnValue + 1;
}

void Denoiser::setVariance(int x, int y, Color3f value) {
    samples->at(x + y * size.x())->push_back(value);
}

void Denoiser::getVarianceFinal() {
    for(int y=0;y<size.y();y++) {
        for(int x=0;x<size.x();x++) {
            float tempSum = 0.f;
            for(int i=0;i<sampleCount;i++) {
                tempSum += pow(samples->at(y * size.x() + x)->at(i).x() - input->coeffRef(y, x).x(),2) +
                           pow(samples->at(y * size.x() + x)->at(i).y() - input->coeffRef(y, x).y(),2) +
                           pow(samples->at(y * size.x() + x)->at(i).z() - input->coeffRef(y, x).z(),2);
            }
            variances->at(y * size.x() + x) = tempSum / (sampleCount-1);
        }
    }
}

void Denoiser::filterSelection() {
    
    std::vector<int>* tempFilterScales;
    tempFilterScales = new std::vector<int>();
    tempFilterScales->reserve(size.x() * size.y());
    for(int i=0;i<size.x() * size.y();i++) {
        tempFilterScales->push_back(0);
    }
    
    for(int x = 0;x<size.x();x++) {
        for(int y=0;y<size.y();y++) {
            if(x < DENOISER_FILTER_SIZE/2 || x >= size.x()-DENOISER_FILTER_SIZE/2) {
                tempFilterScales->at(x + y * size.x()) = filterScales->at(x + y * size.x());
                continue;
            }
            if(y < DENOISER_FILTER_SIZE/2 || y >= size.y()-DENOISER_FILTER_SIZE/2) {
                tempFilterScales->at(x + y * size.x()) = filterScales->at(x + y * size.x());
                continue;
            }
            
            int step = filterScales->at(x + y * size.x()) + 1;
            float tempStep = 0.f;
            
            if(step > DENOISER_FILTER_NUM - 1)
                step -= 1;
            
            for(int i=0;i<DENOISER_FILTER_SIZE;i++) {
                for(int j=0;j<DENOISER_FILTER_SIZE;j++) {
                    if(i == DENOISER_FILTER_SIZE/2 && j == DENOISER_FILTER_SIZE/2)
                        continue;
                    int index = (y + j - DENOISER_FILTER_SIZE/2) * size.x() + x + i - DENOISER_FILTER_SIZE/2;
                    tempStep += filters->at(step)[i + DENOISER_FILTER_SIZE * j]*filterScales->at(index);
                }
            }
            tempFilterScales->at(x + y * size.x()) = (int)tempStep/(filterPdfs->at(step) - filters->at(step)[DENOISER_FILTER_SIZE + DENOISER_FILTER_SIZE*DENOISER_FILTER_SIZE/2]);
        }
    }
    
    delete filterScales;
    filterScales = tempFilterScales;
}

float Denoiser::eval(int x, int y, float stddev, float radius) const {
    float invTwoStddev = 1.f/(stddev * stddev);
    
    float a = INV_TWOPI * 1 * invTwoStddev;
    float b = -(x*x + y*y) * 0.5f * invTwoStddev;
    
    return a * exp(b);
}

Bitmap* Denoiser::getResult() {
    return result;
}

NORI_NAMESPACE_END


