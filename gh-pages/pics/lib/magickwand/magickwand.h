/* MagickWand for PHP module and user visible functions declaration file

   Author: Ouinnel Watson
   Homepage: 
   Current Version: 0.1.3
   Release Date: 2005-01-11
*/

#ifndef PHP_MAGICKWAND_H
#define PHP_MAGICKWAND_H

extern zend_module_entry magickwand_module_entry;
#define phpext_magickwand_ptr &magickwand_module_entry

#ifdef PHP_WIN32
#define PHP_MAGICKWAND_API __declspec(dllexport)
#else
#define PHP_MAGICKWAND_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define MAGICKWAND_VERSION "0.1.3"

/* ************************************************************************************************************** */

	ZEND_MINIT_FUNCTION( magickwand );
/*
	ZEND_MSHUTDOWN_FUNCTION( magickwand );
	ZEND_RINIT_FUNCTION( magickwand );
	ZEND_RSHUTDOWN_FUNCTION( magickwand );
*/
	ZEND_MINFO_FUNCTION( magickwand );

/*  Custom generic Wand exception functions
	-- can be used on any of the 4 MagickWand Wand/Iterator resource types
*/
	ZEND_FUNCTION( wandgetexception );
	ZEND_FUNCTION( wandgetexceptionstring );
	ZEND_FUNCTION( wandgetexceptiontype );
	ZEND_FUNCTION( wandhasexception );

	ZEND_FUNCTION( isdrawingwand );
	ZEND_FUNCTION( ismagickwand );
	ZEND_FUNCTION( ispixeliterator );
	ZEND_FUNCTION( ispixelwand );

	/* DrawingWand functions */

	ZEND_FUNCTION( cleardrawingwand );
	ZEND_FUNCTION( clonedrawingwand );
	ZEND_FUNCTION( destroydrawingwand );
	ZEND_FUNCTION( drawaffine );
	ZEND_FUNCTION( drawannotation );
	ZEND_FUNCTION( drawarc );

	ZEND_FUNCTION( drawbezier );

	ZEND_FUNCTION( drawcircle );
	ZEND_FUNCTION( drawcolor );
	ZEND_FUNCTION( drawcomment );
	ZEND_FUNCTION( drawcomposite );
	ZEND_FUNCTION( drawellipse );
	ZEND_FUNCTION( drawgetclippath );
	ZEND_FUNCTION( drawgetcliprule );
	ZEND_FUNCTION( drawgetclipunits );

	ZEND_FUNCTION( drawgetexception );
	ZEND_FUNCTION( drawgetexceptionstring );
	ZEND_FUNCTION( drawgetexceptiontype );

	ZEND_FUNCTION( drawgetfillalpha );
	ZEND_FUNCTION( drawgetfillcolor );
	ZEND_FUNCTION( drawgetfillopacity );
	ZEND_FUNCTION( drawgetfillrule );
	ZEND_FUNCTION( drawgetfont );
	ZEND_FUNCTION( drawgetfontfamily );
	ZEND_FUNCTION( drawgetfontsize );
	ZEND_FUNCTION( drawgetfontstretch );
	ZEND_FUNCTION( drawgetfontstyle );
	ZEND_FUNCTION( drawgetfontweight );
	ZEND_FUNCTION( drawgetgravity );
	ZEND_FUNCTION( drawgetstrokealpha );
	ZEND_FUNCTION( drawgetstrokeantialias );
	ZEND_FUNCTION( drawgetstrokecolor );
	ZEND_FUNCTION( drawgetstrokedasharray );
	ZEND_FUNCTION( drawgetstrokedashoffset );
	ZEND_FUNCTION( drawgetstrokelinecap );
	ZEND_FUNCTION( drawgetstrokelinejoin );
	ZEND_FUNCTION( drawgetstrokemiterlimit );
	ZEND_FUNCTION( drawgetstrokeopacity );
	ZEND_FUNCTION( drawgetstrokewidth );
	ZEND_FUNCTION( drawgettextalignment );
	ZEND_FUNCTION( drawgettextantialias );
	ZEND_FUNCTION( drawgettextdecoration );
	ZEND_FUNCTION( drawgettextencoding );

	ZEND_FUNCTION( drawgetvectorgraphics );

	ZEND_FUNCTION( drawgettextundercolor );
	ZEND_FUNCTION( drawline );
	ZEND_FUNCTION( drawmatte );
	ZEND_FUNCTION( drawpathclose );
	ZEND_FUNCTION( drawpathcurvetoabsolute );
	ZEND_FUNCTION( drawpathcurvetorelative );
	ZEND_FUNCTION( drawpathcurvetoquadraticbezierabsolute );
	ZEND_FUNCTION( drawpathcurvetoquadraticbezierrelative );
	ZEND_FUNCTION( drawpathcurvetoquadraticbeziersmoothabsolute );
	ZEND_FUNCTION( drawpathcurvetoquadraticbeziersmoothrelative );
	ZEND_FUNCTION( drawpathcurvetosmoothabsolute );
	ZEND_FUNCTION( drawpathcurvetosmoothrelative );
	ZEND_FUNCTION( drawpathellipticarcabsolute );
	ZEND_FUNCTION( drawpathellipticarcrelative );
	ZEND_FUNCTION( drawpathfinish );
	ZEND_FUNCTION( drawpathlinetoabsolute );
	ZEND_FUNCTION( drawpathlinetorelative );
	ZEND_FUNCTION( drawpathlinetohorizontalabsolute );
	ZEND_FUNCTION( drawpathlinetohorizontalrelative );
	ZEND_FUNCTION( drawpathlinetoverticalabsolute );
	ZEND_FUNCTION( drawpathlinetoverticalrelative );
	ZEND_FUNCTION( drawpathmovetoabsolute );
	ZEND_FUNCTION( drawpathmovetorelative );
	ZEND_FUNCTION( drawpathstart );
	ZEND_FUNCTION( drawpoint );

	ZEND_FUNCTION( drawpolygon );

	ZEND_FUNCTION( drawpolyline );

	ZEND_FUNCTION( drawpopclippath );
	ZEND_FUNCTION( drawpopdefs );
	ZEND_FUNCTION( drawpoppattern );
	ZEND_FUNCTION( drawpushclippath );
	ZEND_FUNCTION( drawpushdefs );
	ZEND_FUNCTION( drawpushpattern );
	ZEND_FUNCTION( drawrectangle );
	ZEND_FUNCTION( drawrender );
	ZEND_FUNCTION( drawrotate );
	ZEND_FUNCTION( drawroundrectangle );
	ZEND_FUNCTION( drawscale );
	ZEND_FUNCTION( drawsetclippath );
	ZEND_FUNCTION( drawsetcliprule );
	ZEND_FUNCTION( drawsetclipunits );
	ZEND_FUNCTION( drawsetfillalpha );
	ZEND_FUNCTION( drawsetfillcolor );
	ZEND_FUNCTION( drawsetfillopacity );
	ZEND_FUNCTION( drawsetfillpatternurl );
	ZEND_FUNCTION( drawsetfillrule );
	ZEND_FUNCTION( drawsetfont );
	ZEND_FUNCTION( drawsetfontfamily );
	ZEND_FUNCTION( drawsetfontsize );
	ZEND_FUNCTION( drawsetfontstretch );
	ZEND_FUNCTION( drawsetfontstyle );
	ZEND_FUNCTION( drawsetfontweight );
	ZEND_FUNCTION( drawsetgravity );
	ZEND_FUNCTION( drawsetstrokealpha );
	ZEND_FUNCTION( drawsetstrokeopacity );
	ZEND_FUNCTION( drawsetstrokeantialias );
	ZEND_FUNCTION( drawsetstrokecolor );

	ZEND_FUNCTION( drawsetstrokedasharray );

	ZEND_FUNCTION( drawsetstrokedashoffset );
	ZEND_FUNCTION( drawsetstrokelinecap );
	ZEND_FUNCTION( drawsetstrokelinejoin );
	ZEND_FUNCTION( drawsetstrokemiterlimit );
	ZEND_FUNCTION( drawsetstrokepatternurl );
	ZEND_FUNCTION( drawsetstrokewidth );
	ZEND_FUNCTION( drawsettextalignment );
	ZEND_FUNCTION( drawsettextantialias );
	ZEND_FUNCTION( drawsettextdecoration );
	ZEND_FUNCTION( drawsettextencoding );
	ZEND_FUNCTION( drawsettextundercolor );
	ZEND_FUNCTION( drawsetvectorgraphics );
	ZEND_FUNCTION( drawskewx );
	ZEND_FUNCTION( drawskewy );
	ZEND_FUNCTION( drawtranslate );
	ZEND_FUNCTION( drawsetviewbox );
	ZEND_FUNCTION( newdrawingwand );
	ZEND_FUNCTION( popdrawingwand );
	ZEND_FUNCTION( pushdrawingwand );

	/* MagickWand functions */

	ZEND_FUNCTION( clearmagickwand );
	ZEND_FUNCTION( clonemagickwand );
	ZEND_FUNCTION( destroymagickwand );
	ZEND_FUNCTION( magickadaptivethresholdimage );

/* OW modification from C API convention:
      PHP's magickaddimage adds only the current active image from one wand to another,
	  while PHP's magickaddimages replicates the functionality of the C API's
	  MagickAddImage(), adding all the images of one wnad to another.

	  Minor semantics, but original funtionality proved annoying in actual usage.
*/
	ZEND_FUNCTION( magickaddimage );
	ZEND_FUNCTION( magickaddimages );

	ZEND_FUNCTION( magickaddnoiseimage );
	ZEND_FUNCTION( magickaffinetransformimage );
	ZEND_FUNCTION( magickannotateimage );
	ZEND_FUNCTION( magickappendimages );
	ZEND_FUNCTION( magickaverageimages );
	ZEND_FUNCTION( magickblackthresholdimage );

	ZEND_FUNCTION( magickblurimage );
/*	ZEND_FUNCTION( magickblurimagechannel );  */

	ZEND_FUNCTION( magickborderimage );
	ZEND_FUNCTION( magickcharcoalimage );
	ZEND_FUNCTION( magickchopimage );
	ZEND_FUNCTION( magickclipimage );
	ZEND_FUNCTION( magickclippathimage );
	ZEND_FUNCTION( magickcoalesceimages );
	ZEND_FUNCTION( magickcolorfloodfillimage );
	ZEND_FUNCTION( magickcolorizeimage );
	ZEND_FUNCTION( magickcombineimages );
	ZEND_FUNCTION( magickcommentimage );

	ZEND_FUNCTION( magickcompareimages );
/*	ZEND_FUNCTION( magickcompareimagechannels ); */

	ZEND_FUNCTION( magickcompositeimage );
	ZEND_FUNCTION( magickconstituteimage );
	ZEND_FUNCTION( magickcontrastimage );

	ZEND_FUNCTION( magickconvolveimage );
/*	ZEND_FUNCTION( magickconvolveimagechannel );  */

	ZEND_FUNCTION( magickcropimage );
	ZEND_FUNCTION( magickcyclecolormapimage );
	ZEND_FUNCTION( magickdeconstructimages );
	ZEND_FUNCTION( magickdescribeimage );
	ZEND_FUNCTION( magickdespeckleimage );
	ZEND_FUNCTION( magickdrawimage );

	ZEND_FUNCTION( magickechoimageblob );
	ZEND_FUNCTION( magickechoimagesblob );

	ZEND_FUNCTION( magickedgeimage );
	ZEND_FUNCTION( magickembossimage );
	ZEND_FUNCTION( magickenhanceimage );
	ZEND_FUNCTION( magickequalizeimage );

	ZEND_FUNCTION( magickevaluateimage );
/*	ZEND_FUNCTION( magickevaluateimagechannel );  */

	ZEND_FUNCTION( magickflattenimages );
	ZEND_FUNCTION( magickflipimage );
	ZEND_FUNCTION( magickflopimage );
	ZEND_FUNCTION( magickframeimage );

	ZEND_FUNCTION( magickfximage );
/*	ZEND_FUNCTION( magickfximagechannel ); */

	ZEND_FUNCTION( magickgammaimage );
/*	ZEND_FUNCTION( magickgammaimagechannel ); */

	ZEND_FUNCTION( magickgaussianblurimage );
/*	ZEND_FUNCTION( magickgaussianblurimagechannel ); */

	ZEND_FUNCTION( magickgetcopyright );

	ZEND_FUNCTION( magickgetexception );
	ZEND_FUNCTION( magickgetexceptionstring );
	ZEND_FUNCTION( magickgetexceptiontype );

	ZEND_FUNCTION( magickgetfilename );
	ZEND_FUNCTION( magickgetformat );
	ZEND_FUNCTION( magickgethomeurl );
	ZEND_FUNCTION( magickgetimage );
	ZEND_FUNCTION( magickgetimagebackgroundcolor );
	ZEND_FUNCTION( magickgetimageblob );
	ZEND_FUNCTION( magickgetimagesblob );
	ZEND_FUNCTION( magickgetimageblueprimary );
	ZEND_FUNCTION( magickgetimagebordercolor );

	ZEND_FUNCTION( magickgetimagechannelmean );
	ZEND_FUNCTION( magickgetimagecolormapcolor );
	ZEND_FUNCTION( magickgetimagecolors );
	ZEND_FUNCTION( magickgetimagecolorspace );
	ZEND_FUNCTION( magickgetimagecompose );
	ZEND_FUNCTION( magickgetimagecompression );
	ZEND_FUNCTION( magickgetimagecompressionquality );
	ZEND_FUNCTION( magickgetimagedelay );

	ZEND_FUNCTION( magickgetimagedepth );
/*	ZEND_FUNCTION( magickgetimagechanneldepth ); */

	ZEND_FUNCTION( magickgetimagedistortion );
/*	ZEND_FUNCTION( magickgetimagechanneldistortion ); */

	ZEND_FUNCTION( magickgetimagedispose );

	ZEND_FUNCTION( magickgetimageextrema );
/*	ZEND_FUNCTION( magickgetimagechannelextrema ); */

	ZEND_FUNCTION( magickgetimagefilename );
	ZEND_FUNCTION( magickgetimageformat );
	ZEND_FUNCTION( magickgetimagegamma );
	ZEND_FUNCTION( magickgetimagegreenprimary );
	ZEND_FUNCTION( magickgetimageheight );
	ZEND_FUNCTION( magickgetimagehistogram );
	ZEND_FUNCTION( magickgetimageindex );
	ZEND_FUNCTION( magickgetimageinterlacescheme );
	ZEND_FUNCTION( magickgetimageiterations );
	ZEND_FUNCTION( magickgetimagemattecolor );

	ZEND_FUNCTION( magickgetimagemimetype );
	ZEND_FUNCTION( magickgetmimetype );

	ZEND_FUNCTION( magickgetimagepixels );
	ZEND_FUNCTION( magickgetimageprofile );
	ZEND_FUNCTION( magickgetimageredprimary );
	ZEND_FUNCTION( magickgetimagerenderingintent );
	ZEND_FUNCTION( magickgetimageresolution );
	ZEND_FUNCTION( magickgetimagescene );
	ZEND_FUNCTION( magickgetimagesignature );
	ZEND_FUNCTION( magickgetimagesize );
	ZEND_FUNCTION( magickgetimagetype );
	ZEND_FUNCTION( magickgetimageunits );
	ZEND_FUNCTION( magickgetimagevirtualpixelmethod );
	ZEND_FUNCTION( magickgetimagewhitepoint );
	ZEND_FUNCTION( magickgetimagewidth );
	ZEND_FUNCTION( magickgetinterlacescheme );
	ZEND_FUNCTION( magickgetnumberimages );
	ZEND_FUNCTION( magickgetpackagename );
	ZEND_FUNCTION( magickgetquantumdepth );
	ZEND_FUNCTION( magickgetreleasedate );
	ZEND_FUNCTION( magickgetresourcelimit );
	ZEND_FUNCTION( magickgetsamplingfactors );

/*	ZEND_FUNCTION( magickgetsize ); */
	ZEND_FUNCTION( magickgetwandsize );

	ZEND_FUNCTION( magickgetversion );

/* OW added for convenience in getting the version as a number or string */
	ZEND_FUNCTION( magickgetversionnumber );
	ZEND_FUNCTION( magickgetversionstring );
/* end convenience functions */

	ZEND_FUNCTION( magickhasnextimage );
	ZEND_FUNCTION( magickhaspreviousimage );
	ZEND_FUNCTION( magickimplodeimage );
	ZEND_FUNCTION( magicklabelimage );

	ZEND_FUNCTION( magicklevelimage );
/*	ZEND_FUNCTION( magicklevelimagechannel ); */

	ZEND_FUNCTION( magickmagnifyimage );
	ZEND_FUNCTION( magickmapimage );
	ZEND_FUNCTION( magickmattefloodfillimage );
	ZEND_FUNCTION( magickmedianfilterimage );
	ZEND_FUNCTION( magickminifyimage );
	ZEND_FUNCTION( magickmodulateimage );
	ZEND_FUNCTION( magickmontageimage );
	ZEND_FUNCTION( magickmorphimages );
	ZEND_FUNCTION( magickmosaicimages );
	ZEND_FUNCTION( magickmotionblurimage );

	ZEND_FUNCTION( magicknegateimage );
/*	ZEND_FUNCTION( magicknegateimagechannel ); */

	ZEND_FUNCTION( magicknewimage );

	ZEND_FUNCTION( magicknextimage );
	ZEND_FUNCTION( magicknormalizeimage );
	ZEND_FUNCTION( magickoilpaintimage );

	ZEND_FUNCTION( magickpaintopaqueimage );
	ZEND_FUNCTION( magickpainttransparentimage );

	ZEND_FUNCTION( magickpingimage );
	ZEND_FUNCTION( magickposterizeimage );
	ZEND_FUNCTION( magickpreviewimages );
	ZEND_FUNCTION( magickpreviousimage );
	ZEND_FUNCTION( magickprofileimage );
	ZEND_FUNCTION( magickquantizeimage );
	ZEND_FUNCTION( magickquantizeimages );
	ZEND_FUNCTION( magickqueryconfigureoption );
	ZEND_FUNCTION( magickqueryconfigureoptions );
	ZEND_FUNCTION( magickqueryfontmetrics );

/* the following functions are convenience functions I implemented, as alternatives to calling MagickQueryFontMetrics */
	ZEND_FUNCTION( magickgetcharwidth );
	ZEND_FUNCTION( magickgetcharheight );
	ZEND_FUNCTION( magickgettextascent );
	ZEND_FUNCTION( magickgettextdescent );
	ZEND_FUNCTION( magickgetstringwidth );
	ZEND_FUNCTION( magickgetstringheight );
	ZEND_FUNCTION( magickgetmaxtextadvance );
/* end of convenience functions */

	ZEND_FUNCTION( magickqueryfonts );
	ZEND_FUNCTION( magickqueryformats );
	ZEND_FUNCTION( magickradialblurimage );
	ZEND_FUNCTION( magickraiseimage );
	ZEND_FUNCTION( magickreadimage );
	ZEND_FUNCTION( magickreadimageblob );
	ZEND_FUNCTION( magickreadimagefile );

	/* Custom PHP function; accepts a PHP array of filenames and attempts to read them all into the MagickWand */
	ZEND_FUNCTION( magickreadimages );

	ZEND_FUNCTION( magickreducenoiseimage );
	ZEND_FUNCTION( magickremoveimage );
	ZEND_FUNCTION( magickremoveimageprofile );

	ZEND_FUNCTION( magickremoveimageprofiles );

	ZEND_FUNCTION( magickresetiterator );
	ZEND_FUNCTION( magickresampleimage );
	ZEND_FUNCTION( magickresizeimage );
	ZEND_FUNCTION( magickrollimage );
	ZEND_FUNCTION( magickrotateimage );
	ZEND_FUNCTION( magicksampleimage );
	ZEND_FUNCTION( magickscaleimage );
	ZEND_FUNCTION( magickseparateimagechannel );
	ZEND_FUNCTION( magicksetcompressionquality );
	ZEND_FUNCTION( magicksetfilename );
	ZEND_FUNCTION( magicksetfirstiterator );
	ZEND_FUNCTION( magicksetformat );

	ZEND_FUNCTION( magicksetimage );
	ZEND_FUNCTION( magicksetimagebackgroundcolor );
	ZEND_FUNCTION( magicksetimagebias );
	ZEND_FUNCTION( magicksetimageblueprimary );
	ZEND_FUNCTION( magicksetimagebordercolor );
	ZEND_FUNCTION( magicksetimagecolormapcolor );
	ZEND_FUNCTION( magicksetimagecolorspace );
	ZEND_FUNCTION( magicksetimagecompose );
	ZEND_FUNCTION( magicksetimagecompression );
	ZEND_FUNCTION( magicksetimagecompressionquality );
	ZEND_FUNCTION( magicksetimagedelay );

	ZEND_FUNCTION( magicksetimagedepth );
/*	ZEND_FUNCTION( magicksetimagechanneldepth ); */

	ZEND_FUNCTION( magicksetimagedispose );
	ZEND_FUNCTION( magicksetimagefilename );

	ZEND_FUNCTION( magicksetimageformat );

	ZEND_FUNCTION( magicksetimagegamma );
	ZEND_FUNCTION( magicksetimagegreenprimary );
	ZEND_FUNCTION( magicksetimageindex );
	ZEND_FUNCTION( magicksetimageinterlacescheme );
	ZEND_FUNCTION( magicksetimageiterations );
	ZEND_FUNCTION( magicksetimagemattecolor );
	ZEND_FUNCTION( magicksetimageoption );
	ZEND_FUNCTION( magicksetimagepixels );
	ZEND_FUNCTION( magicksetimageprofile );
	ZEND_FUNCTION( magicksetimageredprimary );
	ZEND_FUNCTION( magicksetimagerenderingintent );
	ZEND_FUNCTION( magicksetimageresolution );
	ZEND_FUNCTION( magicksetimagescene );
	ZEND_FUNCTION( magicksetimagetype );
	ZEND_FUNCTION( magicksetimageunits );
	ZEND_FUNCTION( magicksetimagevirtualpixelmethod );
	ZEND_FUNCTION( magicksetimagewhitepoint );

	ZEND_FUNCTION( magicksetinterlacescheme );
	ZEND_FUNCTION( magicksetlastiterator );
	ZEND_FUNCTION( magicksetpassphrase );
	ZEND_FUNCTION( magicksetresolution );
	ZEND_FUNCTION( magicksetresourcelimit );
	ZEND_FUNCTION( magicksetsamplingfactors );

/*	ZEND_FUNCTION( magicksetsize ); */
	ZEND_FUNCTION( magicksetwandsize );

	ZEND_FUNCTION( magickshadowimage );

	ZEND_FUNCTION( magicksharpenimage );
/*	ZEND_FUNCTION( magicksharpenimagechannel ); */

	ZEND_FUNCTION( magickshaveimage );
	ZEND_FUNCTION( magickshearimage );
	ZEND_FUNCTION( magicksolarizeimage );
	ZEND_FUNCTION( magickspliceimage );
	ZEND_FUNCTION( magickspreadimage );
	ZEND_FUNCTION( magicksteganoimage );
	ZEND_FUNCTION( magickstereoimage );
	ZEND_FUNCTION( magickstripimage );
	ZEND_FUNCTION( magickswirlimage );
	ZEND_FUNCTION( magicktextureimage );

	ZEND_FUNCTION( magickthresholdimage );
/*	ZEND_FUNCTION( magickthresholdimagechannel ); */

	ZEND_FUNCTION( magicktintimage );
	ZEND_FUNCTION( magicktransformimage );
	ZEND_FUNCTION( magicktrimimage );

	ZEND_FUNCTION( magickunsharpmaskimage );
/*	ZEND_FUNCTION( magickunsharpmaskimagechannel ); */

	ZEND_FUNCTION( magickwaveimage );
	ZEND_FUNCTION( magickwhitethresholdimage );
	ZEND_FUNCTION( magickwriteimage );
	ZEND_FUNCTION( magickwriteimagefile );
	ZEND_FUNCTION( magickwriteimages );
	ZEND_FUNCTION( newmagickwand );

	/* PixelIterator functions */

	ZEND_FUNCTION( clearpixeliterator );
	ZEND_FUNCTION( destroypixeliterator );
	ZEND_FUNCTION( newpixeliterator );
	ZEND_FUNCTION( newpixelregioniterator );

	ZEND_FUNCTION( pixelgetiteratorexception );
	ZEND_FUNCTION( pixelgetiteratorexceptionstring );
	ZEND_FUNCTION( pixelgetiteratorexceptiontype );

	ZEND_FUNCTION( pixelgetnextiteratorrow );
	ZEND_FUNCTION( pixelresetiterator );
	ZEND_FUNCTION( pixelsetiteratorrow );
	ZEND_FUNCTION( pixelsynciterator );

	/* PixelWand functions */

	ZEND_FUNCTION( clearpixelwand );
	ZEND_FUNCTION( destroypixelwand );

/************************** OW_Modified *************************
MagickWand's destroypixelwandarray() == ImageMagick's destroypixelwands
*/
/*	ZEND_FUNCTION( destroypixelwands ); */
	ZEND_FUNCTION( destroypixelwandarray );

	ZEND_FUNCTION( newpixelwand );

/************************** OW_Modified *************************
MagickWand's newpixelwandarray() == ImageMagick's newpixelwands
*/
/*	ZEND_FUNCTION( newpixelwands ); */
	ZEND_FUNCTION( newpixelwandarray );

	ZEND_FUNCTION( pixelgetalpha );
	ZEND_FUNCTION( pixelgetalphaquantum );

	ZEND_FUNCTION( pixelgetexception );
	ZEND_FUNCTION( pixelgetexceptionstring );
	ZEND_FUNCTION( pixelgetexceptiontype );

	ZEND_FUNCTION( pixelgetblack );
	ZEND_FUNCTION( pixelgetblackquantum );
	ZEND_FUNCTION( pixelgetblue );
	ZEND_FUNCTION( pixelgetbluequantum );
	ZEND_FUNCTION( pixelgetcolorasstring );
	ZEND_FUNCTION( pixelgetcolorcount );
	ZEND_FUNCTION( pixelgetcyan );
	ZEND_FUNCTION( pixelgetcyanquantum );
	ZEND_FUNCTION( pixelgetgreen );
	ZEND_FUNCTION( pixelgetgreenquantum );
	ZEND_FUNCTION( pixelgetindex );
	ZEND_FUNCTION( pixelgetmagenta );
	ZEND_FUNCTION( pixelgetmagentaquantum );
	ZEND_FUNCTION( pixelgetopacity );
	ZEND_FUNCTION( pixelgetopacityquantum );
	ZEND_FUNCTION( pixelgetquantumcolor );
	ZEND_FUNCTION( pixelgetred );
	ZEND_FUNCTION( pixelgetredquantum );
	ZEND_FUNCTION( pixelgetyellow );
	ZEND_FUNCTION( pixelgetyellowquantum );
	ZEND_FUNCTION( pixelsetalpha );
	ZEND_FUNCTION( pixelsetalphaquantum );
	ZEND_FUNCTION( pixelsetblack );
	ZEND_FUNCTION( pixelsetblackquantum );
	ZEND_FUNCTION( pixelsetblue );
	ZEND_FUNCTION( pixelsetbluequantum );
	ZEND_FUNCTION( pixelsetcolor );
	ZEND_FUNCTION( pixelsetcolorcount );
	ZEND_FUNCTION( pixelsetcyan );
	ZEND_FUNCTION( pixelsetcyanquantum );
	ZEND_FUNCTION( pixelsetgreen );
	ZEND_FUNCTION( pixelsetgreenquantum );
	ZEND_FUNCTION( pixelsetindex );
	ZEND_FUNCTION( pixelsetmagenta );
	ZEND_FUNCTION( pixelsetmagentaquantum );
	ZEND_FUNCTION( pixelsetopacity );
	ZEND_FUNCTION( pixelsetopacityquantum );
	ZEND_FUNCTION( pixelsetquantumcolor );
	ZEND_FUNCTION( pixelsetred );
	ZEND_FUNCTION( pixelsetredquantum );
	ZEND_FUNCTION( pixelsetyellow );
	ZEND_FUNCTION( pixelsetyellowquantum );

#ifdef ZTS
#define MAGICKWAND_G(v) TSRMG(magickwand_globals_id, zend_magickwand_globals *, v)
#else
#define MAGICKWAND_G(v) (magickwand_globals.v)
#endif

#endif
