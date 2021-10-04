#include "MainComponent.h"
#include "../upgrade.h"
#include <fstream>
#include <components/version_label.hpp>
#include <components/look_and_feel/colours.hpp>
#include <components/look_and_feel/fonts.hpp>

MainComponent::MainComponent()
{
    addAndMakeVisible(header);
    configureVersionLabel(versionLabel);
    addAndMakeVisible(versionLabel);
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (EarColours::Background);
    auto area = getLocalBounds();

    auto topHalf = area.removeFromTop(getHeight() / 2);
    topHalf.removeFromTop(30); // margin
    g.setColour (EarColours::Heading);
    g.setFont (EarFonts::HeroHeading);
    g.drawText ("Project Upgrade Utility", topHalf.removeFromTop(topHalf.getHeight() / 2), Justification::centredBottom, true);
    g.setColour (EarColours::Label);
    g.setFont (EarFonts::Label);
    topHalf.removeFromTop(10); // margin
    g.drawText ("Updates REAPER projects to use the latest version of the EAR Production Suite", topHalf, Justification::centredTop, true);

    area.removeFromBottom(50); // version label space
    g.setColour (EarColours::Label);
    g.setFont (EarFonts::Description.italicised());
    if(processing) {
        g.drawText("Please wait. Processing...", area, Justification::centred, true);
    } else {
        g.drawText("Drag & Drop REAPER project files (.RPP) here...", area, Justification::centred, true);
    }
}

void MainComponent::resized()
{
    header.setBounds(getLocalBounds().removeFromTop(50));
    versionLabel.setBounds(getLocalBounds().removeFromBottom(30));
}

bool MainComponent::isInterestedInFileDrag(const StringArray & files)
{
    for(auto& file : files) {
        if(file.endsWithIgnoreCase(".rpp") || file.endsWithIgnoreCase(".rpp-bak")) {
            return true;
        }
    }
    return false;
}

void MainComponent::filesDropped(const StringArray & files, int x, int y)
{
    processing = true;
    repaint();

    int nonRpps = 0;        // Count of files which dont have correct extension
    int successCount = 0;   // Count of files successfully converted
    int failCount = 0;      // Count of files failed conversion
    int nopCount = 0;       // Count of files not requiring conversion
    int changeCount = 0;    // Number of changes in the last successfully converted file

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
                changeCount = upgrade::upgrade(ifs, ofs);
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

    // --- Show Results ---

    juce::String msg;

    if(files.size() == 1) {
        if(successCount > 0) {
            msg = "File upgraded successfully.\n";
            msg += juce::String(changeCount);
            msg += " changes made.\n     (saved with '-conv' appended to filename).";
        } else if(nopCount > 0) {
            msg = "File did not require upgrade.";
        } else if(failCount > 0) {
            msg = "File upgrade failed!";
        }

    } else {
        msg = juce::String(successCount);

        if(successCount == 0) {
            msg += " files upgraded successfully.";
        } else if(successCount == 1) {
            msg += " file upgraded successfully\n     (saved with '-conv' appended to filename).";
        } else {
            msg += " files upgraded successfully\n     (saved with '-conv' appended to filenames).";
        }

        if(nopCount > 0) {
            msg += "\n\n";
            msg += juce::String(nopCount);
            if(nopCount == 1) {
                msg += " file did not require upgrade.";
            } else {
                msg += " files did not require upgrade.";
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
            msg += "\n\nUpgrade failed on ";
            msg += juce::String(failCount);
            if(failCount == 1) {
                msg += " file!";
            } else {
                msg += " files!";
            }
        }
    }

    processing = false;
    repaint();

    NativeMessageBox::showMessageBox(
        failCount > 0? AlertWindow::AlertIconType::WarningIcon : AlertWindow::AlertIconType::InfoIcon,
        "Project Upgrade Results", msg, this);
}
