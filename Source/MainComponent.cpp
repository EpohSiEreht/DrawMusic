#include "MainComponent.h"

#include "Configuration.h"
MainComponent::MainComponent ()
  : gridData_(Configuration::getGridWidth(), Configuration::getGridHeight()), //TODO This janky dependency chain
    colourScheme_(),
    brushPalette_("brushPalette", colourScheme_),
    drawGrid_(gridData_, colourScheme_, brushPalette_),
    deviceManager_(),
    audioSourcePlayer_(),
    transportSource_(),
    gridAudioSource_(gridData_),
    thread_("audio file preview"),
    playStopButton_("playStopButton"),
    clearButton_("renderButton"),
    reconstructionSlider_("reconstructionSlider")
{
    addAndMakeVisible(&brushPalette_);

    addAndMakeVisible(&playStopButton_);
    playStopButton_.setButtonText (TRANS("play"));
    playStopButton_.setConnectedEdges (Button::ConnectedOnRight);
    playStopButton_.addListener (this);

    addAndMakeVisible(&clearButton_);
    clearButton_.setButtonText(TRANS("clear"));
    clearButton_.setConnectedEdges(Button::ConnectedOnLeft);
    clearButton_.addListener(this);

    addAndMakeVisible(&reconstructionSlider_);
    reconstructionSlider_.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    reconstructionSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, 30, 30);
    reconstructionSlider_.setRange(0, 10, 1);

    addAndMakeVisible(&drawGrid_);

    setSize(Configuration::getMainWindowWidth(), Configuration::getMainWindowHeight());

    // Post Size

    // Audio
    deviceManager_.initialise (0, 2, 0, true, String::empty, 0);
    thread_.startThread(3);
    deviceManager_.addAudioCallback(&audioSourcePlayer_);
    audioSourcePlayer_.setSource(&transportSource_);
    transportSource_.setSource(nullptr);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void MainComponent::resized()
{
    drawGrid_.setBounds(10, 10, Configuration::getGridWidth(), Configuration::getGridHeight());
    playStopButton_.setBounds (10, getHeight() - 10 - 24, 150, 24);
    clearButton_.setBounds (10 + 150, (getHeight() - 10 - 24) + 0, 150, 24);
    reconstructionSlider_.setBounds(320, getHeight() - 34, 150, 24);
    brushPalette_.setBounds(getWidth() - 50, getHeight() - 50, 50, 50);
}

void MainComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &playStopButton_)
    {
        togglePlayback();
    }
    else if (buttonThatWasClicked == &clearButton_)
    {
        stopPlayback();
        gridData_.clear();
        drawGrid_.refreshAll();
    }
}

void MainComponent::togglePlayback()
{
    if (transportSource_.isPlaying())
    {
        stopPlayback();
    }
    else
    {
        startPlayback();
    }
}

void MainComponent::startPlayback()
{
    transportSource_.setSource(&gridAudioSource_);
    transportSource_.setPosition (0);
    playStopButton_.setButtonText (TRANS("stop"));
    transportSource_.start();
}

void MainComponent::stopPlayback()
{
    transportSource_.stop();
    transportSource_.setSource(nullptr);
    playStopButton_.setButtonText (TRANS("play"));
}
