#include "AzimuthDialLookAndFeel.h"
#include "BinaryData.h"

//==============================================================================
AzimuthDialLookAndFeel::AzimuthDialLookAndFeel()
{
    const char *imageData = BinaryData::head_png;
    const int imageDatasize = BinaryData::head_pngSize;
    img = juce::ImageCache::getFromMemory(imageData, imageDatasize);
}

//==============================================================================
void AzimuthDialLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y,
                                              int width, int height,
                                              float sliderPosProportional,
                                              float rotaryStartAngle,
                                              float rotaryEndAngle,
                                              juce::Slider &slider)
{

    if (img.isValid())
    {
        // reverse rotation
        float rotation =
            juce::MathConstants<float>::twoPi * (1.0f - sliderPosProportional);
        const float radius = juce::jmin(width / 2.0f, height / 2.0f);
        float imgCentreX = float(img.getWidth()) * .5f;
        float imgCentreY = float(img.getHeight()) * .5f;
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        img = img.rescaled(
            radius, radius,
            juce::Graphics::ResamplingQuality::highResamplingQuality);

        juce::AffineTransform transform;
        float centreImage = float(centerX) - imgCentreX;

        transform = transform.rotation(rotation, imgCentreX, imgCentreY)
                        .translated(centreImage, 0);
        g.drawImageTransformed(img, transform, false);
    }
    else
    {
        static const float textPpercent = 0.35f;
        juce::Rectangle<float> text_bounds(
            1.0f + width * (1.0f - textPpercent) / 2.0f, 0.5f * height,
            width * textPpercent, 0.5f * height);

        g.setColour(juce::Colours::white);

        g.drawFittedText(juce::String("No Image"),
                         text_bounds.getSmallestIntegerContainer(),
                         juce::Justification::horizontallyCentred |
                             juce::Justification::centred,
                         1);
    }
}