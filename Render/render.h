#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include "../init.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QMessageBox>
#include <QtGui/QIntValidator>

#include <iostream>

// Const
// Max depth is the maximum number of bounces a ray can have before destroying itself

namespace render {
    vec3 scene(const Ray& r, vec3& bgCol, Hitable& world, int depth, int useSkyCol) {
        // Make new list of world items
        hitRecord rec;
        if (depth >= Utility::maxDepth) {
            return vec3(0, 0, 0);
        }

        if (!(world.hit(r, 0.001, Utility::maxFloat, rec))) {
            // If no collision, return sky colour
            switch (useSkyCol) {
            case 0:
                return bgCol;
                break;
            case 1:
                return Utility::getFakeScatteringSkyGradient(r);
                break;
            case 2:
                return Utility::getSimpleSkyGradient(r);
                break;
            case 3:
                return Utility::getNightSkyGradient(r);
                break;
            default:
                return bgCol;
            }
        }

        // Else, do our recursive calls
        Ray scatteredRay;
        vec3 attenuation;
        vec3 emissionColour = rec.matPtr->emitted(rec.u, rec.v, rec.pos);

        // Push off the surface normal a little bit (no collision error)
        if (!(rec.matPtr->scatter(r, rec, attenuation, scatteredRay))) {
            return emissionColour;
        }

        vec3 scatterColour = attenuation * scene(scatteredRay, bgCol, world, depth + 1, useSkyCol);
        
        return emissionColour + scatterColour;

        /*
        if (depth < MAXDEPTH && rec.matPtr->scatter(r, rec, attenuation, scatteredRay)) {
            return attenuation * scene(scatteredRay, world, depth + 1);
        } else {
            return vec3(0, 0, 0);
        }*/
    }

    void writeColourToScreen(int imgWidth, int imgHeight, Camera& cam, int x, int y, Hitable& world, int sampleCount, vec3& bgCol, int useSkyCol, std::vector<std::tuple<int, int, int>>& pixelData) {
        // Set UV's
        // We can offset randomly to anti alias cheaply, moving the cam
        vec3 col(0, 0, 0);
        for (int s = 0; s < sampleCount; s++) {
            float u = float(x + (float)rand() / RAND_MAX) / float(imgWidth);
            float v = float(y + (float)rand() / RAND_MAX) / float(imgHeight);

            // Get sky colour
            Ray rayDir = cam.getRay(u, v);
            vec3 pos = rayDir.getPointParam(2.0);
            col += scene(rayDir, bgCol, world, 0, useSkyCol);
        }
        // Divide by sample count
        col /= float(sampleCount);

        // Colour gamma correction
        col = vec3(sqrt(col.getX()), sqrt(col.getY()), sqrt(col.getZ()));

        // Normalize values
        int ir = Utility::clamp(static_cast<int>(255.999 * col.getX()), 0, 255);
        int ig = Utility::clamp(static_cast<int>(255.999 * col.getY()), 0, 255);
        int ib = Utility::clamp(static_cast<int>(255.999 * col.getZ()), 0, 255);

        // Write in real time
        if (x % imgWidth == 1) sdltemplate::loop();

        // Output
        sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
        sdltemplate::drawPoint(x, imgHeight - y);

        // Write to pixel data
        pixelData.emplace_back(ir, ig, ib);
    }

    std::vector<std::tuple<int, int, int>> renderChunk(Scene& parser, Camera& cam, Hitable& worldList, int startRow, int endRow, std::mutex& mutex, bool debugPrint = false) {
        // Return a partial array of the final image
        std::vector<std::tuple<int, int, int>> partialPixelData;

        // Render portion of the image
        for (int y = endRow - 1; y >= startRow; y--) {
            for (int x = 0; x < parser.imageWidth; x++) {
                // Output (remember your thread safety)
                std::lock_guard<std::mutex> lock(mutex);
                writeColourToScreen(parser.imageWidth, parser.imageHeight, cam, x, y, worldList, parser.sampleCount, parser.bgColour, parser.useSkyColour, partialPixelData);

                if (debugPrint) {
                    std::cout << "Rendering pixel " << int(x + y) << std::endl;
                }
            }
        }

        return partialPixelData;
    }

    void renderScene(Scene& parser, Camera& cam, Hitable& worldList, std::vector<std::tuple<int, int, int>>& pixelData, std::mutex& mutex, bool useMultithread = true, bool debugPrint = false) {
        srand((unsigned)time(NULL));

        // Establish SDL Window
        sdltemplate::sdl("Raytracer", parser.imageWidth, parser.imageHeight);
        sdltemplate::loop();
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
        
        if (!useMultithread) {
            // Render the entire scene in a single loop (no threads)
            pixelData = renderChunk(parser, cam, worldList, 0, parser.imageHeight, mutex, debugPrint);
        } else {
            // Get available CPU cores (8)
            const int numThreads = std::thread::hardware_concurrency();

            // Create and launch threads
            // We are splitting the image into 'n' rows instead of using active 'n' cells method
            std::vector<std::thread> threads;
            int rowsPerThread = parser.imageHeight / numThreads;
            int startRow = 0;
            int endRow = rowsPerThread;

            // Vector to store partial pixel data and their start height
            std::vector<std::pair<int, std::vector<std::tuple<int, int, int>>>> partialResults;

            for (int i = 0; i < numThreads; i++) {
                threads.emplace_back([&, i, startRow, endRow]() {
                    std::vector<std::tuple<int, int, int>> partialPixelData = renderChunk(std::ref(parser),
                        std::ref(cam),
                        std::ref(worldList),
                        startRow,
                        endRow,
                        mutex,
                        debugPrint);
                    partialResults.emplace_back(startRow, std::move(partialPixelData));
                    });

                startRow = endRow;
                endRow = (i == numThreads - 2) ? parser.imageHeight : endRow + rowsPerThread;
            }

            // Join threads, wait
            for (std::thread& thread : threads) {
                thread.join();
            }

            // Combine partial results into the final pixelData without sorting, since we know order
            // Parallel reduction -> fix the data race of multiple threads trying to write to pixelData simultaneously
            // Image is reconstructed at the end
            for (const auto& partialResult : partialResults) {
                pixelData.insert(pixelData.end(), partialResult.second.begin(), partialResult.second.end());
            }
        }
    
        // Print time taken
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        Utility::printTimeTaken(startTime, endTime);

        if (debugPrint) {
            std::cout << "Done!" << std::endl;
        }

        // Keep window active
        while (sdltemplate::running) {
            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    sdltemplate::running = false;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        sdltemplate::running = false;
                    }
                }
            }

            sdltemplate::loop();
        }

        SDL_Quit();
    }

    void saveToBMP(const char* filename, int width, int height, const std::vector<std::tuple<int, int, int>>& pixels) {
        std::vector<uint8_t> bmpData; // Create a vector to store the BMP data

        // Convert the RGB pixel data to BMP format (BGR)
        for (const auto& pixel : pixels) {
            uint8_t b = static_cast<uint8_t>(std::get<0>(pixel));
            uint8_t g = static_cast<uint8_t>(std::get<1>(pixel));
            uint8_t r = static_cast<uint8_t>(std::get<2>(pixel));

            bmpData.push_back(b);
            bmpData.push_back(g);
            bmpData.push_back(r);
        }

        // Use stb_image_write to save the BMP file
        if (!stbi_write_bmp(filename, width, height, 3, bmpData.data())) {
            std::cerr << "Failed to save the BMP image." << std::endl;
        }
    }


    bool loadFromBMP(const char* filename, std::vector<std::tuple<int, int, int>>& pixels, int& width, int& height) {
        // Load image
        int channels;
        uint8_t* imgData = stbi_load(filename, &width, &height, &channels, 0);

        if (!imgData) {
            std::cerr << "Failed to load the BMP image." << std::endl;
            return false;
        }

        // Ensure the image has 3 channels (RGB)
        if (channels != 3) {
            std::cerr << "Image is not in RGB format." << std::endl;
            stbi_image_free(imgData);
            return false;
        }

        // Convert the loaded image data into the vector tuple format of ints
        pixels.clear();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Grab index of curr pixel
                int index = (y * width + x) * 3;
                uint8_t b = imgData[index];     // B
                uint8_t g = imgData[index + 1]; // G
                uint8_t r = imgData[index + 2]; // R
                pixels.push_back(std::make_tuple(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
            }
        }

        stbi_image_free(imgData);
        return true;
    }
}

namespace denoise {
    // Extract a patch centered on (i, j) in the unfiltered image
    std::vector<std::tuple<int, int, int>> extractPatch(const std::vector<std::tuple<int, int, int>>& pixels,
                                                        int width,
                                                        int height,
                                                        int i,
                                                        int j,
                                                        int patchRadius) {
        std::vector<std::tuple<int, int, int>> patch;
        
        // Loop through pixel kernel
        // For pixels on the image edge, we clamp and pad the image by stretching the edge pixel
        for (int m = -patchRadius; m <= patchRadius; ++m) {
            for (int n = -patchRadius; n <= patchRadius; ++n) {
                int ni = std::clamp(i + m, 0, height - 1);
                int nj = std::clamp(j + n, 0, width - 1);
                patch.push_back(pixels[ni * width + nj]);
            }
        }
        return patch;
    }

    // Function to calculate variance of pixel color (based on surrounding pixels or external estimation)
    float calculateColorVariance(const std::vector<std::tuple<int, int, int>>& patch) {
        size_t patchSize = patch.size();
        // Handle empty patch case
        if (patchSize == 0) {
            return 0.0f;
        }

        // Compute mean color values
        float rSum = 0.0f, gSum = 0.0f, bSum = 0.0f;
        for (const auto& col : patch) {
            rSum += static_cast<float>(std::get<0>(col));
            gSum += static_cast<float>(std::get<1>(col));
            bSum += static_cast<float>(std::get<2>(col));
        }

        float rMean = rSum / patchSize;
        float gMean = gSum / patchSize;
        float bMean = bSum / patchSize;

        // Compute variance for each color channel
        float rVar = 0.0f, gVar = 0.0f, bVar = 0.0f;
        for (const auto& col : patch) {
            rVar += (std::get<0>(col) - rMean) * (std::get<0>(col) - rMean);
            gVar += (std::get<1>(col) - gMean) * (std::get<1>(col) - gMean);
            bVar += (std::get<2>(col) - bMean) * (std::get<2>(col) - bMean);
        }

        // Normalize by patch size and average across channels
        float avgVariance = (rVar + gVar + bVar) / (3 * patchSize);
        return std::max(0.0f, avgVariance);
    }

    // Grab squared distance between two colour values
    float colDisSquared(const std::tuple<int, int, int>& c1, const std::tuple<int, int, int>& c2) {
        float rDiff = std::get<0>(c1) - std::get<0>(c2);
        float gDiff = std::get<1>(c1) - std::get<1>(c2);
        float bDiff = std::get<2>(c1) - std::get<2>(c2);

        return rDiff * rDiff + gDiff * gDiff + bDiff * bDiff;
    }

    float colDisSquaredRousselle(const std::tuple<int, int, int>& c1,
                                 const std::tuple<int, int, int>& c2,
                                 float varP, 
                                 float varQ, 
                                 float rangeSigma, 
                                 float epsilon) {
        float colourDis = colDisSquared(c1, c2);
        float varPQ = std::min(varP, varQ);

        // See updated distance formula (on linked paper for nlm Rousselle at al RKZ12)
        float dis = ((colourDis - (varP + varPQ)) / ((epsilon + varP + varQ) / (2 * rangeSigma * rangeSigma)));
        return dis;
    }

    float calculateWeight(int i, int j, int ni, int nj,
        const std::vector<std::tuple<int, int, int>>& currPatch,
        const std::vector<std::tuple<int, int, int>>& neighbourPatch,
        float spatialSigma, float rangeSigma, float smoothing) {

        // Calculate spatial distance (Euclidean distance) between (i, j) and (ni, nj)
        float spatialDistance = std::sqrt((i - ni) * (i - ni) + (j - nj) * (j - nj));
        float spatialWeight = std::exp(-spatialDistance * spatialDistance / (2 * spatialSigma * spatialSigma));

        // Calculate patch difference (range similarity) between currPatch and neighbourPatch
        float patchDistance = 0.0f;
        for (size_t k = 0; k < currPatch.size(); ++k) {
            // patchDistance += colDisSquaredRousselle(currPatch[k], neighbourPatch[k], currPatchVariance, neighbourPatchVariance, rangeSigma, epsilon);
            patchDistance += denoise::colDisSquared(currPatch[k], neighbourPatch[k]);
        }
        patchDistance /= static_cast<float>(currPatch.size());
        patchDistance = std::max(0.0f, patchDistance);

        float k = smoothing / 100.0f;
        float rangeWeight = std::exp(-patchDistance / (k * k * 2 * rangeSigma * rangeSigma));

        // Final NLM weight is the product of spatial and range weights
        return spatialWeight * rangeWeight;
    }

    void firstOrderRegression(int width, int height, int patchRadius,
                              float spatialSigma, float rangeSigma, float smoothing, int passIndex, float epsilon,
                              const std::vector<std::tuple<int, int, int>>& unfilteredPixels,
                              std::vector<std::tuple<int, int, int>>& filteredPixels) {
        /*
        np = neighbouring pixel
        Err(a, b, c) = sum(weight) * (np - (a(nx-x) + b(ny-y) + c)^2)
        Partial derivative of a, b, c (product rule)
        dE/da = -2*sum(w) * (nx-x)*(np - (a(nx-x) + b(ny-y) + c)))
        dE/db = -2*sum(w) * (ny-y)*(np - (a(nx-x) + b(ny-y) + c)))
        dE/dc = -2*sum(w) * (np - (a(nx-x) + b(ny-y) + c)))
        Expand then represent in matrix form A.x = B
        x = [a, b, c]
        A = 
        [sum(w)*(nx-x)^2 sum(w)*(nx-x)*(ny-y) sum(w)*(nx-x)]
        [sum(w)*(nx-x)*(ny-y) sum(w)*(ny-y)^2 sum(w)*(ny-y)]
        [sum(w)*(nx-x) sum(w)*(nx-y) sum(w)]
        B = 
        [sum(x)*(nx-x)*np]
        [sum(x)*(ny-y)*np]
        [sum(x)*np]

        since take first row for example
        sum(x)*(nx-x)*np = dE/da 
                         = -2*sum(w) * (nx-x)*(np - (a(nx-x) + b(ny-y) + c))) 
                         = a*sum(w)*(nx-x)^2 + b*sum(w)*(nx-x)*(ny-y) +c*sum(w)*(nx-x)

        To solve, accumulate weighted sums across neighbours to build 3x3 matrix A
        then, solve for a, b, c once determinant is not zero using Cramer's rule

        */
        
        // Calculate weights using non-local means
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Coefficients for regression for each colour channel (r, g, b)
                // for linear model: pixel ~= a(nx-x) + b(ny-y) + c
                // a, b are spatial coefficients, c is intercept constant term
                float aR = 0, bR = 0, cR = 0;
                float aG = 0, bG = 0, cG = 0;
                float aB = 0, bB = 0, cB = 0;

                // Weighted least squares sums for rgb channels
                // sumWx2 = sum(w) * dx^2, w = weight, dx = delta(nx-x)
                // sumWxy = sum(w) * dx * dy
                // sumWxpR/G/B - w*dx/dy/1*p for R/G/B
                // sumW = sum of all w to normalise
                float sumWx2 = 0, sumWy2 = 0, sumWxy = 0;
                float sumWxpR = 0, sumWypR = 0, sumWpR = 0;
                float sumWxpG = 0, sumWypG = 0, sumWpG = 0;
                float sumWxpB = 0, sumWypB = 0, sumWpB = 0;
                float sumW = 0;

                // Target pixel's intensity (as tuple RGB)
                auto [targetR, targetG, targetB] = unfilteredPixels[y * width + x];

                // Loop through the neighborhood defined by patchRadius
                for (int dy = -patchRadius; dy <= patchRadius; ++dy) {
                    for (int dx = -patchRadius; dx <= patchRadius; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            // Neighboring pixel
                            auto [nr, ng, nb] = unfilteredPixels[ny * width + nx];

                            // Compute weight based on distance and range similarity
                            float spatialDist2 = dx * dx + dy * dy;
                            float rangeDist2 = std::pow(targetR - nr, 2) + std::pow(targetG - ng, 2) + std::pow(targetB - nb, 2);
                            float k = smoothing / 100.0f;
                            float weight = std::exp(-spatialDist2 / (2 * spatialSigma * spatialSigma) - rangeDist2 / (k * k * 2 * rangeSigma * rangeSigma));

                            // Accumulate weighted sums for regression (weight = similarity)
                            // Notice no use of patches compared to zero order regression to compute variance
                            sumWx2 += weight * dx * dx;
                            sumWy2 += weight * dy * dy;
                            sumWxy += weight * dx * dy;

                            sumWxpR += weight * dx * nr;
                            sumWypR += weight * dy * nr;
                            sumWpR += weight * nr;

                            sumWxpG += weight * dx * ng;
                            sumWypG += weight * dy * ng;
                            sumWpG += weight * ng;

                            sumWxpB += weight * dx * nb;
                            sumWypB += weight * dy * nb;
                            sumWpB += weight * nb;

                            sumW += weight;
                        }
                    }
                }

                // Don't solve for 3x3 matrix, we solve for 2x2 submatrices since c is a constant
                /*
                A =
                [sumw*x^2 sumw*x*y sumw*x]
                [sumw*x*y sumw*y^2 sumw*y]
                [sumw*x sumw*y sumw]
                =
                [sumWx2 sumWxy sumx*y]
                [sumWxy sumWy2 sumx*y]
                [sumw*x sumw*y sumw]
                = 
                [sumWx2 sumWxy]
                [sumWxy sumWy2]
                det(A) = sumWx2 * sumWy2 - sumWxy^2;
                */
                // Solve for rgb channels coefficients
                // Intercept gives pixel value at currPixel
                // Ax = B if det =/= 0 then unique solution, otherwise we default to average sum
                float det = sumWx2 * sumWy2 - sumWxy * sumWxy;
                if (std::abs(det) > epsilon) {
                    // Solve for aR, bR (Cramer's Rule)
                    aR = (sumWy2 * sumWxpR - sumWxy * sumWypR) / det;
                    bR = (sumWx2 * sumWypR - sumWxy * sumWxpR) / det;
                    cR = (sumWpR - aR * 0 - bR * 0) / sumW;

                    aG = (sumWy2 * sumWxpG - sumWxy * sumWypG) / det;
                    bG = (sumWx2 * sumWypG - sumWxy * sumWxpG) / det;
                    cG = (sumWpG - aG * 0 - bG * 0) / sumW;

                    aB = (sumWy2 * sumWxpB - sumWxy * sumWypB) / det;
                    bB = (sumWx2 * sumWypB - sumWxy * sumWxpB) / det;
                    cB = (sumWpB - aB * 0 - bB * 0) / sumW;
                } else {
                    cR = sumWpR / sumW;
                    cG = sumWpG / sumW;
                    cB = sumWpB / sumW;
                }

                // Set the denoised pixel color for the current pixel
                if (passIndex == 1) {
                    filteredPixels[y * width + x] = std::make_tuple(static_cast<int>(cB), static_cast<int>(cG), static_cast<int>(cR));
                } else {
                    filteredPixels[y * width + x] = std::make_tuple(static_cast<int>(cR), static_cast<int>(cG), static_cast<int>(cB));
                }
            }
        }
    }

    void zeroOrderRegression(int width, int height, int patchRadius,
                             float spatialSigma, float rangeSigma, float smoothing, int passIndex, float epsilon,
                             const std::vector<std::tuple<int, int, int>>& unfilteredPixels,
                             std::vector<std::tuple<int, int, int>>& filteredPixels) {
        // Calculate weights using non-local means
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Grab patch at pixel (x, y)
                std::vector<std::tuple<int, int, int>> currPatch = denoise::extractPatch(unfilteredPixels,
                    width,
                    height,
                    y,
                    x,
                    patchRadius);

                // Sums of color channels
                float rSum = 0.0f;
                float gSum = 0.0f;
                float bSum = 0.0f;
                float weightSum = 0.0f;

                // Loop over neighbors within patch radius
                for (int dy = -patchRadius; dy <= patchRadius; ++dy) {
                    for (int dx = -patchRadius; dx <= patchRadius; ++dx) {
                        int ny = std::clamp(y + dy, 0, height - 1);
                        int nx = std::clamp(x + dx, 0, width - 1);

                        // Extract neighboring patch and calculate weight
                        std::vector<std::tuple<int, int, int>> neighbourPatch = denoise::extractPatch(unfilteredPixels,
                            width,
                            height,
                            ny,
                            nx,
                            patchRadius);

                        float currWeight = calculateWeight(y, x, ny, nx, currPatch, neighbourPatch, spatialSigma, rangeSigma, smoothing);
                        weightSum += currWeight;

                        // Accumulate weighted color values
                        std::tuple<int, int, int> origCol = unfilteredPixels[ny * width + nx];
                        rSum += currWeight * std::get<0>(origCol);
                        gSum += currWeight * std::get<1>(origCol);
                        bSum += currWeight * std::get<2>(origCol);
                    }
                }

                // Normalize color values by the total weight sum
                rSum /= weightSum;
                gSum /= weightSum;
                bSum /= weightSum;

                // Adjust RGB order based on pass index
                if (passIndex == 1) {
                    filteredPixels[y * width + x] = std::make_tuple(static_cast<int>(bSum),
                        static_cast<int>(gSum),
                        static_cast<int>(rSum));
                } else {
                    filteredPixels[y * width + x] = std::make_tuple(static_cast<int>(rSum),
                        static_cast<int>(gSum),
                        static_cast<int>(bSum));
                }
            }
        }
    }
}

class MainWindow : public QMainWindow {
public:
    std::vector<std::tuple<int, int, int>> pixelData;
    int unfilteredPixelsWidth, unfilteredPixelsHeight;
    std::vector<std::tuple<int, int, int>> unfilteredPixels;
    std::vector<std::tuple<int, int, int>> firstPassPixels;
    std::vector<std::tuple<int, int, int>> secondPassPixels;
    std::mutex mutexLock;

    int imageWidth;
    int imageHeight;
    int sampleCount;
    int patchRadius;
    float spatialSigma;
    float rangeSigma;
    float smoothing;
    std::string sceneName;
    std::string renderFileName;
    std::string renderFilePath;
    bool useMultithread;

    QLineEdit* widthInput;
    QLineEdit* heightInput;
    QLineEdit* sampleCountInput;
    QLineEdit* kernelSizeInput;
    QLineEdit* spatialSigmaInput;
    QLineEdit* rangeSigmaInput;
    QLineEdit* smoothingInput;
    QButtonGroup* sceneGroup;
    QLabel* sceneFilePathLabel;
    QLabel* renderFilePathLabel;
    QRadioButton* singleThreadedButton;
    QRadioButton* multiThreadedButton;

    // Todo: move this into a utility file or data file
    static inline std::map<int, std::string> sceneMapping = {
        {1, "Cornell Box"},
        {2, "Infinite Spheres on Checkerboard"},
        {3, "Spheres and Cubes in Shadow"},
        {4, "Material/Texture Showcase"},
        {5, "Debug Scene"}
    };

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        QTabWidget* tabWidget = new QTabWidget(this);

        // Tab 1: User Input
        QWidget* inputTab = new QWidget();
        QVBoxLayout* inputLayout = new QVBoxLayout(inputTab);

        QLabel* sizeLabel = new QLabel("Render Output options:");
        inputLayout->addWidget(sizeLabel);

        int defaultImageWidth = 400;
        int defaultImageHeight = 400;
        int defaultSampleCount = 5;
        widthInput = new QLineEdit(QString::number(defaultImageWidth));
        heightInput = new QLineEdit(QString::number(defaultImageHeight));
        sampleCountInput = new QLineEdit(QString::number(defaultSampleCount));
        imageWidth = defaultImageWidth;
        imageHeight = defaultImageHeight;
        sampleCount = defaultSampleCount;

        // Ensure values are purely numerical
        QIntValidator* validator = new QIntValidator(1, 10000, this);
        widthInput->setValidator(validator);
        heightInput->setValidator(validator);
        sampleCountInput->setValidator(validator);

        connect(widthInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(heightInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(sampleCountInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);

        inputLayout->addWidget(new QLabel("Image Width:"));
        inputLayout->addWidget(widthInput);
        inputLayout->addWidget(new QLabel("Image Height:"));
        inputLayout->addWidget(heightInput);
        inputLayout->addWidget(new QLabel("Sample Count:"));
        inputLayout->addWidget(sampleCountInput);

        // Threading options
        QLabel* threadingLabel = new QLabel("Processing options:");
        inputLayout->addWidget(threadingLabel);

        QButtonGroup* threadingGroup = new QButtonGroup(inputTab);
        singleThreadedButton = new QRadioButton("Single Threaded");
        multiThreadedButton = new QRadioButton("Multithreaded");
        multiThreadedButton->setChecked(true);
        useMultithread = true;
        connect(singleThreadedButton, &QRadioButton::toggled, this, &MainWindow::updateSettings);
        connect(multiThreadedButton, &QRadioButton::toggled, this, &MainWindow::updateSettings);

        threadingGroup->addButton(singleThreadedButton);
        threadingGroup->addButton(multiThreadedButton);

        inputLayout->addWidget(singleThreadedButton);
        inputLayout->addWidget(multiThreadedButton);

        // Tab 2: Premade Scene Selection
        QWidget* premadeSceneTab = new QWidget();
        QVBoxLayout* sceneLayout = new QVBoxLayout(premadeSceneTab);
        sceneLayout->addWidget(new QLabel("Choose from Premade Scenes:"));
        sceneGroup = new QButtonGroup(premadeSceneTab);

        for (const auto& pair : sceneMapping) {
            QString qstr = QString::fromUtf8(pair.second.c_str());
            QRadioButton* radioButton = new QRadioButton(qstr);
            radioButton->setProperty("sceneId", pair.first);
            connect(radioButton, &QRadioButton::toggled, this, &MainWindow::updateSceneName);

            sceneLayout->addWidget(radioButton);
            sceneGroup->addButton(radioButton);
        }

        QPushButton* startRenderButtonPremade = new QPushButton("Start Render");
        sceneLayout->addWidget(startRenderButtonPremade);

        // Tab 3: Load Scene from JSON
        QWidget* userSceneTab = new QWidget();
        QVBoxLayout* txtLayout = new QVBoxLayout(userSceneTab);

        QPushButton* loadFileButton = new QPushButton("Load Scene File");
        sceneFilePathLabel = new QLabel("No file selected.");
        connect(loadFileButton, &QPushButton::clicked, this, &MainWindow::openLoadedScene);

        QPushButton* startRenderButtonLoaded = new QPushButton("Start Render");
        txtLayout->addWidget(new QLabel("Load Custom Scene:"));
        txtLayout->addWidget(loadFileButton);
        txtLayout->addWidget(sceneFilePathLabel);
        txtLayout->addWidget(startRenderButtonLoaded);

        // Connect tabs and buttons
        connect(startRenderButtonPremade, &QPushButton::clicked, this, &MainWindow::startPremadeRender);
        connect(startRenderButtonLoaded, &QPushButton::clicked, this, &MainWindow::startLoadedRender);

        // Tab 4: Denoiser
        QWidget* denoiseTab = new QWidget();
        QVBoxLayout* denoiseLayout = new QVBoxLayout(denoiseTab);

        QPushButton* loadRenderButton = new QPushButton("Load Render Output");
        renderFilePathLabel = new QLabel("No file selected.");
        connect(loadRenderButton, &QPushButton::clicked, this, &MainWindow::openLoadedRender);

        patchRadius = 3; // radius of 3 == 7x7
        smoothing = 5.0f;
        spatialSigma = 0.0f;
        rangeSigma = 100.0f;
        kernelSizeInput = new QLineEdit(QString::number(patchRadius));
        spatialSigmaInput = new QLineEdit(QString::number(static_cast<int>(spatialSigma)));
        rangeSigmaInput = new QLineEdit(QString::number(static_cast<int>(rangeSigma)));
        smoothingInput = new QLineEdit(QString::number(static_cast<int>(smoothing)));
        kernelSizeInput->setValidator(validator);
        spatialSigmaInput->setValidator(validator);
        rangeSigmaInput->setValidator(validator);
        smoothingInput->setValidator(validator);

        connect(kernelSizeInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(spatialSigmaInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(rangeSigmaInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);
        connect(smoothingInput, &QLineEdit::textChanged, this, &MainWindow::updateSettings);

        denoiseLayout->addWidget(new QLabel("Kernel Size: (Higher = Slower)"));
        denoiseLayout->addWidget(kernelSizeInput);
        denoiseLayout->addWidget(new QLabel("Spatial Sigma: (Predicted as image variance, sensitivity of pixel colour distance)"));
        denoiseLayout->addWidget(spatialSigmaInput);
        denoiseLayout->addWidget(new QLabel("Range Sigma: (Sensitivity of pixel spatial distance)"));
        denoiseLayout->addWidget(rangeSigmaInput);
        denoiseLayout->addWidget(new QLabel("Smoothing Filter Strength (% of 100)"));
        denoiseLayout->addWidget(smoothingInput);

        QPushButton* startDenoiseButton = new QPushButton("Start Denoise");
        denoiseLayout->addWidget(new QLabel("Load Render Output (BMP):"));
        denoiseLayout->addWidget(loadRenderButton);
        denoiseLayout->addWidget(renderFilePathLabel);
        denoiseLayout->addWidget(startDenoiseButton);

        // Connect tabs and buttons
        connect(startDenoiseButton, &QPushButton::clicked, this, &MainWindow::startDenoise);

        // Add tabs to window
        tabWidget->addTab(inputTab, "Render Settings");
        tabWidget->addTab(premadeSceneTab, "Premade Scenes");
        tabWidget->addTab(userSceneTab, "Load Scene");
        tabWidget->addTab(denoiseTab, "Denoise Render");

        // Set central widget
        setCentralWidget(tabWidget);

        // Stylesheets
        QString darkStyle = R"(
    QMainWindow {
        background-color: #121212;
        color: #ffffff;
    }
    
    QWidget {
        background-color: #1e1e1e;
        color: #ffffff;
    }
    
    QLabel {
        color: #ffffff;
    }

    QLineEdit {
        background-color: #333333;
        color: #ffffff;
        border: 1px solid #444444;
        border-radius: 5px;
        padding: 5px;
    }
    
    QLineEdit:focus {
        border: 1px solid #0078d7; /* Focused color */
    }

    QPushButton {
        background-color: #444444;
        color: #ffffff;
        border: 1px solid #555555;
        border-radius: 5px;
        padding: 5px;
    }
    
    QPushButton:hover {
        background-color: #555555;
    }
    
    QPushButton:pressed {
        background-color: #666666;
    }
    
    QRadioButton {
        color: #ffffff;
    }
    
    QRadioButton::indicator {
        background-color: #333333;
        border: 1px solid #444444;
    }
    
    QRadioButton::indicator:checked {
        background-color: #0078d7;
        border: 1px solid #0078d7;
    }

    QButtonGroup {
        margin-top: 10px;
    }
    
    QTabWidget::pane {
        background: #1e1e1e; /* Background for the tab widget */
        border: 1px solid #444444;
    }

    QTabBar::tab {
        background: #333333;
        color: #ffffff;
        padding: 10px;
    }
    
    QTabBar::tab:selected {
        background: #0078d7;
        color: #ffffff;
    }
    
    QProgressBar {
        border: 1px solid #555555;
        border-radius: 5px;
        background: #444444;
    }

    QProgressBar::chunk {
        background-color: #0078d7;
        border-radius: 5px;
    }
)";
        this->setStyleSheet(darkStyle);
    }

private:
    void promptSaveImage() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Save Image", "Would you like to save the image to a file?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "Images (*.bmp);;All Files (*)");
            if (!fileName.isEmpty()) {
                const char* name = fileName.toUtf8().constData();
                render::saveToBMP(name, imageWidth, imageHeight, pixelData);
            }
        }
    }

private slots:
    void updateSceneName() {
        QAbstractButton* checkedButton = sceneGroup->checkedButton();
        if (checkedButton) {
            sceneName = checkedButton->text().toUtf8().constData();
        }
    }

    void updateSettings() {
        imageWidth = widthInput->text().toInt();
        imageHeight = heightInput->text().toInt();
        sampleCount = sampleCountInput->text().toInt();
        useMultithread = multiThreadedButton->isChecked();
        patchRadius = kernelSizeInput->text().toInt();
        spatialSigma = static_cast<float>(spatialSigmaInput->text().toInt());
        rangeSigma = static_cast<float>(rangeSigmaInput->text().toInt());
        smoothing = static_cast<float>(smoothingInput->text().toInt());
    }

    void startPremadeRender() {
        std::cout << "Rendering premade scene: " << sceneName
            << " with width: " << imageWidth
            << ", height: " << imageHeight
            << ", samples: " << sampleCount << std::endl;

        int index = 0;
        for (const auto& pair : sceneMapping) {
            if (pair.second == sceneName) {
                index = pair.first;
                break;
            }
        }

        Scene scene = Scene();
        Camera cam = scene.generateSceneFromMapping(index, imageWidth, imageHeight, sampleCount);
        HitableList worldList = scene.worldList;
        render::renderScene(scene, cam, worldList, pixelData, mutexLock, useMultithread);

        promptSaveImage();
    }

    void startLoadedRender() {
        std::cout << "Rendering loaded scene: " << sceneName
            << " with width: " << imageWidth
            << ", height: " << imageHeight
            << ", samples: " << sampleCount << std::endl;

        SceneParser parser = SceneParser();
        Camera cam = parser.generateScene(sceneName, true);
        HitableList worldList = parser.worldList;
        render::renderScene(parser, cam, worldList, pixelData, mutexLock, useMultithread);

        promptSaveImage();
    }

    void openLoadedScene() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open Scene File", "", "Text Files (*.txt)");
        if (!filePath.isEmpty()) {
            QFileInfo fileInfo(filePath);
            QString fileName = fileInfo.fileName();
            sceneFilePathLabel->setText(fileName);
            sceneName = fileName.toUtf8().constData();
        }
    }

    void openLoadedRender() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open Render File", "", "BMP Files (*.bmp)");
        if (!filePath.isEmpty()) {
            unfilteredPixels.clear();
            firstPassPixels.clear();
            secondPassPixels.clear();

            QFileInfo fileInfo(filePath);
            QString fileName = fileInfo.fileName();
            QString filePath = fileInfo.absoluteFilePath();
            renderFilePathLabel->setText(fileName); 
            renderFileName = fileName.toUtf8().constData();
            renderFilePath = filePath.toUtf8().constData();

            // Load BMP file into pixel array
            if (render::loadFromBMP(renderFilePath.c_str(), unfilteredPixels, unfilteredPixelsWidth, unfilteredPixelsHeight)) {
                std::cout << "Image loaded successfully!" << std::endl;
                std::cout << "Width: " << unfilteredPixelsWidth << ", Height: " << unfilteredPixelsHeight << std::endl;
            } else {
                std::cout << "Failed to load the image." << std::endl;
                return;
            }

            // Populate and predict spatial and range sigmas
            // Range sigma is estimated to be ~5% of image variance
            firstPassPixels.resize(unfilteredPixelsWidth * unfilteredPixelsHeight);
            secondPassPixels.resize(unfilteredPixelsWidth * unfilteredPixelsHeight);
            spatialSigma = denoise::calculateColorVariance(unfilteredPixels);
            spatialSigmaInput->setText(QString::number(static_cast<int>(spatialSigma)));
            rangeSigma = 0.05 * spatialSigma;
            rangeSigmaInput->setText(QString::number(static_cast<int>(rangeSigma)));
        }
    }

    void startDenoise() {
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

        /*
        We use the techniques in the paper "Nonlinearly Weighted First-order Regression for
        Denoising Monte Carlo Renderings" to denoise our monte-carlo render. 
        https://benedikt-bitterli.me/nfor/

        We do so by have two passes. 
        First pass calculates pixel weights using non-local means then uses first-order 
        regression to compute denoised value. We use a zero order regression as testing results show that
        it can have greater smoothing over large areas with difficulty preserving edges.
        Second pass uses variance of first pass as regression weights, doing the same
        first-order regression to remove residual artefacts. We use first order regression as large 
        surfaces are smoothed and we just want to preserve edges.

        NLM = non-local means (pixel weight based on similarity rather than spatial closeness)
        */

        const float epsilon = 0.0001;

        // Pixel spatial and similarity sensitivity
        // spatialSigma = how much physical distance affects regression weights, could keep as image variance?
        // rangeSigma = variance of pixel colour difference, low % of variance

        /// FIRST PASS
        std::cout << "Starting first pass with rangeSigma " << rangeSigma << ", spatialSigma " << spatialSigma << ", smoothing " << smoothing << " and kernel size " << patchRadius << std::endl;
        denoise::zeroOrderRegression(unfilteredPixelsWidth, unfilteredPixelsHeight, 
                                   patchRadius, static_cast<float>(spatialSigma), static_cast<float>(rangeSigma),
                                   static_cast<float>(smoothing), 1, epsilon, unfilteredPixels, firstPassPixels);

        /// SECOND PASS
        spatialSigma = denoise::calculateColorVariance(firstPassPixels);
        std::cout << "Starting second pass with rangeSigma " << rangeSigma << ", spatialSigma " << spatialSigma << ", smoothing " << smoothing << " and kernel size " << patchRadius << std::endl;
        denoise::firstOrderRegression(unfilteredPixelsWidth, unfilteredPixelsHeight,
                                   patchRadius, static_cast<float>(spatialSigma), static_cast<float>(rangeSigma),
                                   static_cast<float>(smoothing), 2, epsilon, firstPassPixels, secondPassPixels);

        // Timekeeping
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        Utility::printTimeTaken(startTime, endTime);

        // Save final image
        std::string pathCopy = renderFileName;
        std::string suffix = "_denoised_k" + std::to_string(patchRadius) + 
                             "_rS" + std::to_string(static_cast<int>(rangeSigma)) + 
                             "_sS" + std::to_string(static_cast<int>(spatialSigma)) +
                             "_s" + std::to_string(static_cast<int>(smoothing));
        size_t dotPos = pathCopy.find_last_of('.');
        pathCopy.insert(dotPos, suffix);

        render::saveToBMP(pathCopy.c_str(), unfilteredPixelsWidth, unfilteredPixelsHeight, secondPassPixels);
        std::cout << "Finished and exported to " << pathCopy << std::endl;
    }
};

#endif