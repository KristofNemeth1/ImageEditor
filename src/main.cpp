#include <iostream>
#include <sstream>

#include "Image.hpp"
#include "BlendMode.hpp"
#include "filter/Filter.hpp"

int main() {
    // Base images
    SourceImage horse("horse.jpg");
    SourceImage background1("image-3.png");
    SourceImage background2("image-3.png");

    std::cout << "--- Constructing Pipeline Done (Zero Heap Allocations) ---\n" << std::endl;

    Filter::Inverse inverseHorse(horse);
    BlendMode::Multiply finalComposition(inverseHorse, background1, 50);

    Filter::Blur blurring(finalComposition, 5.0f);

    std::cout << "--- Executing Render Chain ---" << std::endl;
    // 3. Combine them all into a final composition and render
    // C++17 Class Template Argument Deduction (CTAD) automatically infers the deep nested types!
    
    Image finalResult = blurring.render();

    finalResult.Export("output.png");

    return 0;
}