/*
  ==============================================================================

    Defines.h
    Created: 6 Jun 2024 9:04:15pm
    Author:  Nicola

  ==============================================================================
*/

#pragma once

//===================================GLOBAL MEASURE===================================
#define globalWindowWidth 1192
#define globalWindowHeight 696
//===================================TITLE DIEMNSIONS=================================
#define everyTitleHeight 90
#define everyTitleWidth 15
//===================================TITLE POSITIONS==================================
//-----------------------Distortion---------------------------------------
#define distortionTitleX 132
#define distortionTitleY 15
//-----------------------Delay-------------------------------------------
#define delayTitleX 490
#define delayTitleY 15
//-----------------------Reverb------------------------------------------
#define revebtTitleX 132
#define reverbTitleY 360
//-----------------------Eq----------------------------------------------
#define eqTitleX 490
#define eqTitleY 360



//=================================SELECTION EFFECT RECTANGLE========================
#define selectionEffectWidth 332
#define selectinoEffectHeight 323
#define selectionBorderThickness 3
//-----------------------Distortion---------------------------------------
#define distortionSelectionBoxX 12
#define distortionSelectionBoxY 10
//-----------------------Delay-------------------------------------------
#define delaySelectionBoxX 369
#define delaySelectionBoxY 10
//-----------------------Reverb------------------------------------------
#define reverbSelectionBoxX 12
#define reverbSelectionBoxY 358
//-----------------------Eq----------------------------------------------
#define eqSelectionBoxX 369
#define eqSelectionBoxY 358


//================================GRAPHIC DRAW======================================
//-------------------------------side square lenght-----------------------
#define graphicSideLenght 75

//------------------------------MEASURE--------------------------------
#define everyDrawY 110
//------------------------------soft------------------------------------
#define softDistortionDrawX 35
//------------------------------mid------------------------------------------
#define midDistortionDrawX 135
//------------------------------hard----------------------------------------
#define hardDistortionDrawX 235

//=================================VIEWER & SPECTRUM===============================================
#define everyBlackImageX 729
#define everyWidth 445
#define everyHeight 183
#define everyBlackImageRoundness 4

//---------------------------------graphic arrows----------------------------------------
#define everyArrowGraphicThickness 1.0f
#define everyArrowGraphicHeadWidth 7.0f
#define everyArrowGraphicHeadLenght 3.5f
//--------------------------------Viewer---------------------------------------------------
#define viewerBlackImageY 90
//--------------------------------Graphic Axis---------------------------------------------
#define viewerXLineGraphicStartX 739
#define viewerXLineGraphicStartY 265 
#define viewerXLineGraphicEndX 1161
#define viewerXLineGrahpicEndY 265

#define viewerYLineGraphicStartX 739
#define viewerYLineGraphicStartY 265
#define viewerYLineGraphicEndX 739
#define viewerYLineGraphicEndY 100
//------------------------------Graphic viewew Number Value-------------------------------
#define viewerEveryNumemberX 728
#define viewerHighNumberY 100
#define viewerLowNumberY 259
#define viewerEveryNumberLenght 10


//--------------------------------Spectrum------------------------------------------------
#define spectrumBlackImageY 420

//-------------------------------Graphic Axis--------------------------------------------
#define spectrumXLineGraphicStartX 985
#define spectrumXLineGraphicStartY 595
#define spectrumYLineGraphicEndX 1161
#define spectrumYLineGraphicEndY 595

#define spectrumEveryLineGraphicX 739
#define spectrumXLineGraphicY 595
#define spectrumXLineGraphicEndX 930
#define spectrumYLineGraphicStartY 525
#define spectrumYLineGraphicStartArrow 507

#define spectrumGraphicWordsHeight 100
#define spectrumGraphicWordsLenght 11

#define spectrumFreqStartX 907
#define spectrumFreqStartY 591
#define spectrumDbStartX 710
#define spectrumDBStartY 647

//===============================OBJECT SIZE===============================================
//--------------------------------ToggleSize-----------------------------------------------
#define everyToggleSide 100

//--------------------------------Distortion-----------------------------------------------
//------------------------Toggle----------------------------------------------------------
#define everyDistortionToggleY 40

#define softDistortionToggleX 35
#define midDistortionToggleX 135 
#define hardDistortionToggleX 235

//-----------------------Labels------------------------------------------------------------
#define everyDistortionTypeLabelY 90

#define softDistortionTypeLabelX 30
#define midDistortionTypeLabelX 140
#define hardDistortionTypeLabelX 250

//-----------------------Borders-----------------------------------------------------------
#define everyDistortionBorderY 205

#define dirtortionBorderWidth 85
#define distortionBorderHeight 120

#define distortionGainBorderX 30
#define distortionOffsetBorderX 130 
#define distortionThresholdBorderX 230




#define everyRotarySliderDim 65
#define everyDistortionGainY 232
#define everyDistortionLabelBalueY 272

//-------------------------------Distortion-----------------------------------------------
//----------------Slider------------------------------------------------------------------
#define distortionGainSliderPosX 40
#define distortionOffsetSliderPosX 140
#define treshHoldSliderPosX 240

//----------------ValueLabel--------------------------------------------------------------
#define distortionGainLabelValueX 45
#define distortionOffsetLabelValueX 155
#define distortionThreshHoldLabelValueX 255
