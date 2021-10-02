/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "../upgrade.h"
#include <fstream>

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colour(18, 18, 18));

    g.setColour (Colours::white);
    auto area = getLocalBounds();

    auto topHalf = area.removeFromTop(getHeight() / 2);
    g.setFont (Font (16.0f).boldened());
    g.drawText ("EAR Production Suite - Project Conversion Utility", topHalf.removeFromTop(topHalf.getHeight() / 2), Justification::centredBottom, true);
    g.setFont (Font (12.0f));
    topHalf.removeFromTop(10);
    g.drawText ("Update REAPER projects to use the latest version of the EAR Production Suite", topHalf, Justification::centredTop, false);

    g.setFont (Font (16.0f).italicised());
    g.drawText ("Drag & Drop files here for conversion...", area, Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

bool MainComponent::isInterestedInFileDrag(const StringArray & files)
{
    bool validFileFound = false;

    for(auto& file : files) {
        if(file.endsWithIgnoreCase(".rpp") || file.endsWithIgnoreCase(".rpp-bak")) {
            validFileFound = true;
            break;
        }
    }

    return validFileFound;
}

void MainComponent::filesDropped(const StringArray & files, int x, int y)
{
    int nonRpps = 0;
    int successCount = 0;
    int failCount = 0;
    int nopCount = 0;

    for(auto& file : files) {
        if(file.endsWithIgnoreCase(".rpp") || file.endsWithIgnoreCase(".rpp-bak")) {
            bool cleanupRequired = false;
            auto dotPos = file.lastIndexOf(".");
            juce::String outFilename;
            int suffixNum = 1;
            do {
                outFilename = file.substring(0, dotPos);
                outFilename += "-conv";
                if(suffixNum > 1) {
                    outFilename += "(";
                    outFilename += suffixNum;
                    outFilename += ")";
                }
                suffixNum++;
                outFilename += file.substring(dotPos);
            } while(juce::File(outFilename).exists());
            auto ifs = std::ifstream{file.toStdString().c_str()};
            bool ifsGood = ifs.good();
            auto ofs = std::ofstream{outFilename.toStdString().c_str()};
            bool ofsGood = ofs.good();
            if(!ifsGood || !ofsGood) {
                failCount++;
                cleanupRequired = true;
            } else {
                auto changeCount = upgrade::upgrade(ifs, ofs);
                if(changeCount == 0) {
                    nopCount++;
                    cleanupRequired = true;
                } else {
                    successCount++;
                }
            }
            if(cleanupRequired) {
                // Close file stream and delete output file
                if(ofsGood) ofs.close();
                juce::File(outFilename).deleteFile();
            }
        } else {
            nonRpps++;
        }

    }


    auto msg = juce::String(successCount);
    if(successCount == 0) {
        msg += " files converted successfully.";
    } else if(successCount == 1) {
        msg += " file converted successfully\n     (saved with '-conv' appended to filename).";
    } else {
        msg += " files converted successfully\n     (saved with '-conv' appended to filenames).";
    }

    if(nopCount > 0) {
        msg += "\n\n";
        msg += juce::String(nopCount);
        if(nopCount == 1) {
            msg += " file did not require conversion.";
        } else {
            msg += " files did not require conversion.";
        }
    }

    if(nonRpps > 0) {
        msg += "\n\n";
        msg += juce::String(nonRpps);
        if(nonRpps == 1) {
            msg += " file was not processed as it did not have a .RPP or .RPP-BAK extension.";
        } else {
            msg += " files were not processed as they did not have a .RPP or .RPP-BAK extension.";
        }
    }

    if(failCount > 0) {
        msg += "\n\nConversion failed on ";
        msg += juce::String(failCount);
        if(failCount == 1) {
            msg += " file!";
        } else {
            msg += " files!";
        }
    }

    NativeMessageBox::showMessageBox(failCount > 0? AlertWindow::AlertIconType::WarningIcon : AlertWindow::AlertIconType::InfoIcon, "Conversion Results", msg, this);
}
