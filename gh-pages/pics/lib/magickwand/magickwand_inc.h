
/* MagickWand for PHP main includes file / custom macro definitions file

   Author: Ouinnel Watson
   Homepage: 
   Current Version: 0.1.3
   Release Date: 2005-01-11
*/

#include <math.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
/*
#include "php_ini.h"
*/
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"

#include "wand/magick_wand.h"

#include "magickwand_im_defs.h"

#include "magickwand.h"

/* #define MW_DEBUG */
#undef MW_DEBUG

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (1)
#endif

#define MW_E_ERROR    E_USER_ERROR    /* E_ERROR   */
#define MW_E_WARNING  E_USER_WARNING  /* E_WARNING */
#define MW_E_NOTICE   E_USER_NOTICE   /* E_NOTICE  */

/* True global resources - no need for thread safety here */
static int
	le_DrawingWand,
	le_MagickWand,
	le_PixelIterator,
	le_PixelWand,
	le_PixelIteratorPixelWand;

static char
	*DrawingWand_RSRC_DESC   = "DrawingWand resource",
	*MagickWand_RSRC_DESC    = "MagickWand resource",
	*PixelIterator_RSRC_DESC = "PixelIterator resource",
	*PixelWand_RSRC_DESC     = "PixelWand resource",
	*PixelIteratorPixelWand_RSRC_DESC = "PixelIterator PixelWand resource";

static const double MW_MaxRGB = (double) (MaxRGB);

static MagickBooleanType
		   MW_zend_register_resource(
			   const MagickBooleanType verify, const void *obj_ptr, zval *zval_p, const int le_zend_rsrc_type, int *return_rsrc_id )
{
	TSRMLS_FETCH();

	if ( verify == MagickFalse ) { return MagickFalse; }

	if ( return_rsrc_id == (int *) NULL ) {
		ZEND_REGISTER_RESOURCE( zval_p, (void *) obj_ptr, le_zend_rsrc_type );
	}
	else {
		*return_rsrc_id = ZEND_REGISTER_RESOURCE( zval_p, (void *) obj_ptr, le_zend_rsrc_type );
	}

	return MagickTrue;
}

static MagickBooleanType
		   MW_zend_fetch_resource( zval **rsrc_zval_pp, const int le_zend_rsrc_type, void **img_mgck_ptr )
{
	int id, le_actual_rsrc_type;
	void *resource;

	TSRMLS_FETCH();

	if ( Z_TYPE_PP(rsrc_zval_pp) != IS_RESOURCE ) {
		zend_error( MW_E_ERROR, "%s(): supplied argument is not a valid resource", get_active_function_name(TSRMLS_C) );
		return MagickFalse;
	}

	id = Z_LVAL_PP(rsrc_zval_pp);
	le_actual_rsrc_type = -1;

	resource = (void *) zend_list_find(id, &le_actual_rsrc_type);

	if ( le_actual_rsrc_type == -1 || resource == (void *) NULL ) {
		zend_error( MW_E_ERROR, "%s(): %d is not a valid resource", get_active_function_name(TSRMLS_C), id );
		return MagickFalse;
	}

	if ( le_actual_rsrc_type != le_zend_rsrc_type ) {
		return MagickFalse;
	}

	*img_mgck_ptr = resource;
	return MagickTrue;
}

/* ************************************************************************************************************** */

#ifdef MW_DEBUG
#	define MW_PRINT_DEBUG_INFO		zend_error( MW_E_NOTICE, "Entered %s()...", get_active_function_name( TSRMLS_C ) );
#	define MW_DEBUG_NOTICE( str )	zend_error( MW_E_NOTICE, str " on C source line %d of file %s", __LINE__, __FILE__ );
#	define MW_DEBUG_NOTICE_EX( str, var )	zend_error( MW_E_NOTICE, str " on C source line %d of file %s", var, __LINE__, __FILE__ );
#else
#	define MW_PRINT_DEBUG_INFO
#	define MW_DEBUG_NOTICE( str )
#	define MW_DEBUG_NOTICE_EX( str, var )
#endif

/* ************************************************************************************************************** */
/* RETURN_MW_EMPTY_STRING() was created to resolve an issue with the RETURN_EMPTY_STRING() Zend macro, which caused
   some issues in Windows (re: _empty_string Zend variable)
*/
#define RETVAL_MW_EMPTY_STRING()	RETVAL_STRINGL( "", 0, 1 );

#define RETURN_MW_EMPTY_STRING()	RETVAL_MW_EMPTY_STRING(); return;

/* ************************************************************************************************************** */
/* The macros in this section allow for easy output of simple error strings (containing no format specifiers)
*/
#define MW_SPIT_ERR_STR( ZendErrorType, str )  zend_error( ZendErrorType, "%s(): %s", get_active_function_name( TSRMLS_C ), str );

#define MW_SPIT_FATAL_ERR( str )  MW_SPIT_ERR_STR( MW_E_ERROR, str );

#define MW_SPIT_WARNING( str )  MW_SPIT_ERR_STR( MW_E_WARNING, str );

#define MW_SPIT_NOTICE( str )  MW_SPIT_ERR_STR( MW_E_NOTICE, str );

/* ************************************************************************************************************** */
/* This macro checks for NULL pointers, like those that could be returned from emalloc()
*/
#define MW_NULL_PNTER_ERR_CHECK( type, ptr, err_msg )	if ( ptr == (type*) NULL ) { MW_SPIT_FATAL_ERR( err_msg ); return; }

/* ************************************************************************************************************** */

#define MW_EMALLOC( type, pntr )  \
	pntr = (type*) emalloc( sizeof(type) );  \
	MW_NULL_PNTER_ERR_CHECK( type, pntr, "could not allocate memory for " #type " pointer (" #type "*)" );

#define MW_ARR_ECALLOC( type, arr, arr_len )  \
	arr = (type*) ecalloc( (size_t) arr_len, sizeof(type) );  \
	MW_NULL_PNTER_ERR_CHECK( type, arr, "could not allocate memory for " #type " array (" #type "*)" );

#define MW_EFREE_MEM( Type, mem )  if ( mem != (Type) NULL ) {  efree( mem );  }

/* ************************************************************************************************************** */

#define MW_FREE_MAGICK_MEM( Type, mem )  if ( mem != (Type) NULL ) {  mem = MagickRelinquishMemory( mem );  }

/* ************************************************************************************************************** */

#define DrawingWand_IS_GOOD( wnd   )  IsDrawingWand(   (wnd) )
#define MagickWand_IS_GOOD( wnd    )  IsMagickWand(    (wnd) )
#define PixelIterator_IS_GOOD( wnd )  IsPixelIterator( (wnd) )
#define PixelWand_IS_GOOD( wnd     )  IsPixelWand(     (wnd) )

#define PixelIteratorPixelWand_IS_GOOD( wnd )  IsPixelWand( (wnd) )

#define MW_CHECK_IF_GOOD_SPIT_ERR( ResType, obj )  \
	if ( ResType ## _IS_GOOD( obj ) == MagickFalse ) {  \
		MW_SPIT_FATAL_ERR( #ResType " pointer contained in resource is invalid" ); return;  \
	}

/* ************************************************************************************************************** */

#define MW_DEFINE_PHP_RSRC( ResType )  \
	le_ ## ResType = zend_register_list_destructors_ex( ResType ## _destruction_handler, NULL, ResType ## _RSRC_DESC, module_number );

#define MW_REGISTER_LONG_CONSTANT( TheConstant )  \
	REGISTER_LONG_CONSTANT( "MW_" #TheConstant, (long) TheConstant, CONST_PERSISTENT );

#define MW_REGISTER_DOUBLE_CONSTANT( TheConstant )  \
	REGISTER_DOUBLE_CONSTANT( "MW_" #TheConstant, (double) TheConstant, CONST_PERSISTENT );

/* ************************************************************************************************************** */

#define MW_ZEND_REGISTER_RESOURCE( ResType, obj, zval_p, return_rsrc_id )  \
	MW_zend_register_resource( ResType ## _IS_GOOD( obj ), (void *) obj, zval_p, le_ ## ResType, return_rsrc_id )

/* ************************************************************************************************************** */

#define DrawingWand_CLEAR_EXCEPTION(   wnd )  DrawClearException(          wnd )
#define MagickWand_CLEAR_EXCEPTION(    wnd )  MagickClearException(        wnd )
#define PixelWand_CLEAR_EXCEPTION(     wnd )  PixelClearException(         wnd )
#define PixelIterator_CLEAR_EXCEPTION( wnd )  PixelClearIteratorException( wnd )

#define MW_FETCH_RSRC( ResType, obj, obj_rsrc_zval_pp )  \
	MW_zend_fetch_resource( obj_rsrc_zval_pp, le_ ## ResType, (void *) &(obj) )

#define PRV_GET_N_CHECK_WAND( WandType, wnd, wnd_rsrc_zval_pp, extra_msg_str_literal )  \
	if (   MW_FETCH_RSRC( WandType, wnd, wnd_rsrc_zval_pp ) == MagickFalse  \
		|| WandType ## _IS_GOOD( wnd ) == MagickFalse ) {  \
		MW_SPIT_FATAL_ERR( "function requires a " #WandType " resource" extra_msg_str_literal );  \
		return;  \
	}  \
	WandType ## _CLEAR_EXCEPTION( wnd );
/*	wnd = (WandType *) (wnd);  */

#define DrawingWand_RETRIEVE(   wnd, wnd_rsrc_zval_pp )  PRV_GET_N_CHECK_WAND( DrawingWand,   wnd, wnd_rsrc_zval_pp, "" );
#define MagickWand_RETRIEVE(    wnd, wnd_rsrc_zval_pp )  PRV_GET_N_CHECK_WAND( MagickWand,    wnd, wnd_rsrc_zval_pp, "" );
#define PixelIterator_RETRIEVE( wnd, wnd_rsrc_zval_pp )  PRV_GET_N_CHECK_WAND( PixelIterator, wnd, wnd_rsrc_zval_pp, "" );

#define PixelWand_RETRIEVE( wnd, wnd_rsrc_zval_pp )  \
	if (  (   MW_FETCH_RSRC( PixelWand, wnd, wnd_rsrc_zval_pp ) == MagickFalse  \
		   && MW_FETCH_RSRC( PixelIteratorPixelWand, wnd, wnd_rsrc_zval_pp ) == MagickFalse )  \
		|| IsPixelWand( (wnd) ) == MagickFalse ) {  \
		MW_SPIT_FATAL_ERR( "function requires a PixelWand resource" );  \
		return;  \
	}  \
	PixelWand_CLEAR_EXCEPTION( wnd );
/*	wnd = (PixelWand *) (wnd);  */

#define MW_GET_POINTER_FROM_RSRC( ResType, obj, obj_rsrc_zval_pp )  ResType ## _RETRIEVE( obj, obj_rsrc_zval_pp );

/* This specifically for the DestroyPixelWand*() functions -- can't have them trying to destroy PixelIterator PixelWands */
#define MW_GET_ONLY_PixelWand_POINTER_FROM_RSRC( pxl_wnd, pxl_wnd_rsrc_zvl_pp )  \
	PRV_GET_N_CHECK_WAND( PixelWand, pxl_wnd, pxl_wnd_rsrc_zvl_pp,  \
						  "; (NOTE: PixelWands derived from PixelIterators are also invalid)" );

/* ************************************************************************************************************** */

#define DrawingWand_DESTRUCT_FUNC(   wnd )  DestroyDrawingWand(   wnd );
#define MagickWand_DESTRUCT_FUNC(    wnd )  DestroyMagickWand(    wnd );
#define PixelIterator_DESTRUCT_FUNC( wnd )  DestroyPixelIterator( wnd );
#define PixelWand_DESTRUCT_FUNC(     wnd )  DestroyPixelWand(     wnd );

#define PRV_DESTROY_WAND( WandType, rsrc_ptr ) {  \
	WandType *wnd = (WandType *) (rsrc_ptr);  \
	if ( WandType ## _IS_GOOD( wnd ) == MagickTrue ) {  \
		rsrc_ptr = (void *) WandType ## _DESTRUCT_FUNC( wnd );  \
	}  \
}

#define DrawingWand_DESTROY(   rsrc_ptr )  PRV_DESTROY_WAND( DrawingWand,   (rsrc_ptr) );
#define MagickWand_DESTROY(    rsrc_ptr )  PRV_DESTROY_WAND( MagickWand,    (rsrc_ptr) );
#define PixelIterator_DESTROY( rsrc_ptr )  PRV_DESTROY_WAND( PixelIterator, (rsrc_ptr) );
#define PixelWand_DESTROY(     rsrc_ptr )  PRV_DESTROY_WAND( PixelWand,     (rsrc_ptr) );

#define PixelIteratorPixelWand_DESTROY( rsrc_ptr )    ;
/*  There is no effective PixelIteratorPixelWand_DESTROY() macro as the purpose of the
	le_PixelIteratorPixelWand "type" is to make sure that certain PixelWands are NOT destroyed by
	PHP directly, rather they are destroyed by DestroyPixelIterator() when the PixelIterator
	destruction handler is called from within PHP.
	This macro is here for completeness, in case an attempt is made to destroy one this way.
*/

#define MW_DESTROY_RSRC( ResType, rsrc_ptr )  ResType ## _DESTROY( (rsrc_ptr) );

/* ************************************************************************************************************** */

#define MW_SET_RET_RSRC( ResType, pntr )  \
	if ( pntr == (ResType *) NULL || MW_ZEND_REGISTER_RESOURCE( ResType, pntr, return_value, (int *) NULL ) == MagickFalse ) {  \
		 RETURN_FALSE;  \
	}

#define MW_SET_RET_RSRC_FROM_FUNC( ResType, FunctionCall ) {  \
	ResType *wnd;  \
	wnd = (ResType *) FunctionCall;  \
	MW_SET_RET_RSRC( ResType, wnd );  \
}

/* ************************************************************************************************************** */

#define PRV_IS_AlignType( x ) (x == LeftAlign || x == CenterAlign || x == RightAlign)  /* || x == UndefinedAlign */
#define PRV_IS_ChannelType( x ) (x == RedChannel || x == CyanChannel || x == GreenChannel || x == MagentaChannel || x == BlueChannel || x == YellowChannel || x == AlphaChannel || x == OpacityChannel || x == BlackChannel || x == IndexChannel || x == AllChannels)  /* || x == UndefinedChannel */
#define PRV_IS_ClipPathUnits( x ) (x == UserSpace || x == UserSpaceOnUse || x == ObjectBoundingBox)  /* || x == UndefinedPathUnits */
#define PRV_IS_ColorspaceType( x ) (x == RGBColorspace || x == GRAYColorspace || x == TransparentColorspace || x == OHTAColorspace || x == LABColorspace || x == XYZColorspace || x == YCbCrColorspace || x == YCCColorspace || x == YIQColorspace || x == YPbPrColorspace || x == YUVColorspace || x == CMYKColorspace || x == sRGBColorspace || x == HSBColorspace || x == HSLColorspace || x == HWBColorspace)  /* || x == UndefinedColorspace */
#define PRV_IS_CompositeOperator( x ) (x == NoCompositeOp || x == AddCompositeOp || x == AtopCompositeOp || x == BlendCompositeOp || x == BumpmapCompositeOp || x == ClearCompositeOp || x == ColorBurnCompositeOp || x == ColorDodgeCompositeOp || x == ColorizeCompositeOp || x == CopyBlackCompositeOp || x == CopyBlueCompositeOp || x == CopyCompositeOp || x == CopyCyanCompositeOp || x == CopyGreenCompositeOp || x == CopyMagentaCompositeOp || x == CopyOpacityCompositeOp || x == CopyRedCompositeOp || x == CopyYellowCompositeOp || x == DarkenCompositeOp || x == DstAtopCompositeOp || x == DstCompositeOp || x == DstInCompositeOp || x == DstOutCompositeOp || x == DstOverCompositeOp || x == DifferenceCompositeOp || x == DisplaceCompositeOp || x == DissolveCompositeOp || x == ExclusionCompositeOp || x == HardLightCompositeOp || x == HueCompositeOp || x == InCompositeOp || x == LightenCompositeOp || x == LuminizeCompositeOp || x == MinusCompositeOp || x == ModulateCompositeOp || x == MultiplyCompositeOp || x == OutCompositeOp || x == OverCompositeOp || x == OverlayCompositeOp || x == PlusCompositeOp || x == ReplaceCompositeOp || x == SaturateCompositeOp || x == ScreenCompositeOp || x == SoftLightCompositeOp || x == SrcAtopCompositeOp || x == SrcCompositeOp || x == SrcInCompositeOp || x == SrcOutCompositeOp || x == SrcOverCompositeOp || x == SubtractCompositeOp || x == ThresholdCompositeOp || x == XorCompositeOp)  /* || x == UndefinedCompositeOp */
#define PRV_IS_CompressionType( x ) (x == NoCompression || x == BZipCompression || x == FaxCompression || x == Group4Compression || x == JPEGCompression || x == LosslessJPEGCompression || x == LZWCompression || x == RLECompression || x == ZipCompression)  /* || x == UndefinedCompression */
#define PRV_IS_DecorationType( x ) (x == NoDecoration || x == UnderlineDecoration || x == OverlineDecoration || x == LineThroughDecoration)  /* || x == UndefinedDecoration */
#define PRV_IS_DisposeType( x ) (x == NoneDispose || x == BackgroundDispose || x == PreviousDispose || x == UnrecognizedDispose)  /* || x == UndefinedDispose */
#define PRV_IS_ExceptionType( x ) (x == WarningException || x == ResourceLimitWarning || x == TypeWarning || x == OptionWarning || x == DelegateWarning || x == MissingDelegateWarning || x == CorruptImageWarning || x == FileOpenWarning || x == BlobWarning || x == StreamWarning || x == CacheWarning || x == CoderWarning || x == ModuleWarning || x == DrawWarning || x == ImageWarning || x == WandWarning || x == MonitorWarning || x == RegistryWarning || x == ConfigureWarning || x == ErrorException || x == ResourceLimitError || x == TypeError || x == OptionError || x == DelegateError || x == MissingDelegateError || x == CorruptImageError || x == FileOpenError || x == BlobError || x == StreamError || x == CacheError || x == CoderError || x == ModuleError || x == DrawError || x == ImageError || x == WandError || x == MonitorError || x == RegistryError || x == ConfigureError || x == FatalErrorException || x == ResourceLimitFatalError || x == TypeFatalError || x == OptionFatalError || x == DelegateFatalError || x == MissingDelegateFatalError || x == CorruptImageFatalError || x == FileOpenFatalError || x == BlobFatalError || x == StreamFatalError || x == CacheFatalError || x == CoderFatalError || x == ModuleFatalError || x == DrawFatalError || x == ImageFatalError || x == WandFatalError || x == MonitorFatalError || x == RegistryFatalError || x == ConfigureFatalError)  /* || x == UndefinedException */
#define PRV_IS_FillRule( x ) (x == EvenOddRule || x == NonZeroRule)  /* || x == UndefinedRule */
#define PRV_IS_FilterTypes( x ) (x == PointFilter || x == BoxFilter || x == TriangleFilter || x == HermiteFilter || x == HanningFilter || x == HammingFilter || x == BlackmanFilter || x == GaussianFilter || x == QuadraticFilter || x == CubicFilter || x == CatromFilter || x == MitchellFilter || x == LanczosFilter || x == BesselFilter || x == SincFilter)  /* || x == UndefinedFilter */
#define PRV_IS_GravityType( x ) (x == ForgetGravity || x == NorthWestGravity || x == NorthGravity || x == NorthEastGravity || x == WestGravity || x == CenterGravity || x == EastGravity || x == SouthWestGravity || x == SouthGravity || x == SouthEastGravity || x == StaticGravity)  /* || x == UndefinedGravity */
#define PRV_IS_ImageType( x ) (x == BilevelType || x == GrayscaleType || x == GrayscaleMatteType || x == PaletteType || x == PaletteMatteType || x == TrueColorType || x == TrueColorMatteType || x == ColorSeparationType || x == ColorSeparationMatteType || x == OptimizeType)  /* || x == UndefinedType */
#define PRV_IS_InterlaceType( x ) (x == NoInterlace || x == LineInterlace || x == PlaneInterlace || x == PartitionInterlace)  /* || x == UndefinedInterlace */
#define PRV_IS_LineCap( x ) (x == ButtCap || x == RoundCap || x == SquareCap)  /* || x == UndefinedCap */
#define PRV_IS_LineJoin( x ) (x == MiterJoin || x == RoundJoin || x == BevelJoin)  /* || x == UndefinedJoin */
#define PRV_IS_MagickEvaluateOperator( x ) (x == AddEvaluateOperator || x == AndEvaluateOperator || x == DivideEvaluateOperator || x == LeftShiftEvaluateOperator || x == MaxEvaluateOperator || x == MinEvaluateOperator || x == MultiplyEvaluateOperator || x == OrEvaluateOperator || x == RightShiftEvaluateOperator || x == SetEvaluateOperator || x == SubtractEvaluateOperator || x == XorEvaluateOperator)  /* || x == UndefinedEvaluateOperator */
#define PRV_IS_MetricType( x ) (x == MeanAbsoluteErrorMetric || x == MeanSquaredErrorMetric || x == PeakAbsoluteErrorMetric || x == PeakSignalToNoiseRatioMetric || x == RootMeanSquaredErrorMetric)  /* || x == UndefinedMetric */
#define PRV_IS_MontageMode( x ) (x == FrameMode || x == UnframeMode || x == ConcatenateMode)  /* || x == UndefinedMode */
#define PRV_IS_NoiseType( x ) (x == UniformNoise || x == GaussianNoise || x == MultiplicativeGaussianNoise || x == ImpulseNoise || x == LaplacianNoise || x == PoissonNoise)  /* || x == UndefinedNoise */
#define PRV_IS_PaintMethod( x ) (x == PointMethod || x == ReplaceMethod || x == FloodfillMethod || x == FillToBorderMethod || x == ResetMethod)  /* || x == UndefinedMethod */
#define PRV_IS_PreviewType( x ) (x == RotatePreview || x == ShearPreview || x == RollPreview || x == HuePreview || x == SaturationPreview || x == BrightnessPreview || x == GammaPreview || x == SpiffPreview || x == DullPreview || x == GrayscalePreview || x == QuantizePreview || x == DespecklePreview || x == ReduceNoisePreview || x == AddNoisePreview || x == SharpenPreview || x == BlurPreview || x == ThresholdPreview || x == EdgeDetectPreview || x == SpreadPreview || x == SolarizePreview || x == ShadePreview || x == RaisePreview || x == SegmentPreview || x == SwirlPreview || x == ImplodePreview || x == WavePreview || x == OilPaintPreview || x == CharcoalDrawingPreview || x == JPEGPreview)  /* || x == UndefinedPreview */
#define PRV_IS_RenderingIntent( x ) (x == SaturationIntent || x == PerceptualIntent || x == AbsoluteIntent || x == RelativeIntent)  /* || x == UndefinedIntent */
#define PRV_IS_ResolutionType( x ) (x == PixelsPerInchResolution || x == PixelsPerCentimeterResolution)  /* || x == UndefinedResolution */
#define PRV_IS_ResourceType( x ) (x == AreaResource || x == DiskResource || x == FileResource || x == MapResource || x == MemoryResource)  /* || x == UndefinedResource */
#define PRV_IS_StorageType( x ) (x == CharPixel || x == ShortPixel || x == IntegerPixel || x == LongPixel || x == FloatPixel || x == DoublePixel)  /* || x == UndefinedPixel */
#define PRV_IS_StretchType( x ) (x == NormalStretch || x == UltraCondensedStretch || x == ExtraCondensedStretch || x == CondensedStretch || x == SemiCondensedStretch || x == SemiExpandedStretch || x == ExpandedStretch || x == ExtraExpandedStretch || x == UltraExpandedStretch || x == AnyStretch)  /* || x == UndefinedStretch */
#define PRV_IS_StyleType( x ) (x == NormalStyle || x == ItalicStyle || x == ObliqueStyle || x == AnyStyle)  /* || x == UndefinedStyle */
#define PRV_IS_VirtualPixelMethod( x ) (x == ConstantVirtualPixelMethod || x == EdgeVirtualPixelMethod || x == MirrorVirtualPixelMethod || x == TileVirtualPixelMethod)  /* || x == UndefinedVirtualPixelMethod */

/* ************************************************************************************************************** */

#define PRV_PARSE_COND_START   if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC,

#define PRV_PARSE_COND_END       ) == FAILURE ) { MW_SPIT_FATAL_ERR( "error in function call" ); return; }

/* ************************************************************************************************************** */

#define MW_GET_1_ARG( spec_str, var_1 )  \
	PRV_PARSE_COND_START spec_str, var_1 PRV_PARSE_COND_END

#define MW_GET_2_ARG( spec_str, var_1, var_2 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2 PRV_PARSE_COND_END

#define MW_GET_3_ARG( spec_str, var_1, var_2, var_3 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3 PRV_PARSE_COND_END

#define MW_GET_4_ARG( spec_str, var_1, var_2, var_3, var_4 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4 PRV_PARSE_COND_END

#define MW_GET_5_ARG( spec_str, var_1, var_2, var_3, var_4, var_5 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5 PRV_PARSE_COND_END

#define MW_GET_6_ARG( spec_str, var_1, var_2, var_3, var_4, var_5, var_6 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5, var_6 PRV_PARSE_COND_END

#define MW_GET_7_ARG( spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7 PRV_PARSE_COND_END

#define MW_GET_8_ARG( spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8 PRV_PARSE_COND_END

#define MW_GET_9_ARG( spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9 PRV_PARSE_COND_END

#define MW_GET_10_ARG( spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 PRV_PARSE_COND_END

#define MW_GET_11_ARG( spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_11 )  \
	PRV_PARSE_COND_START spec_str, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_11 PRV_PARSE_COND_END

/* ************************************************************************************************************** */

/* The macros in this section are used in the Is*() functions
*/
#define PRV_START_GET_WAND_VERIFY( WandType )  \
	WandType *wnd;  \
	int arg_count = ZEND_NUM_ARGS();  \
	zval **zvl_pp_args_arr[ 1 ];  \
\
	if ( arg_count != 1 ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): error in function call: function requires exactly 1 parameter",  \
					get_active_function_name(TSRMLS_C) );  \
		return;  \
	}  \
	if ( zend_get_parameters_array_ex( 1, zvl_pp_args_arr ) == FAILURE ) {  \
		zend_error( MW_E_ERROR, "%s(): unknown error in function call", get_active_function_name(TSRMLS_C) );  \
		return;  \
	}  \
	if ( Z_TYPE_PP( zvl_pp_args_arr[0] ) == IS_RESOURCE &&

#define PRV_END_GET_WAND_VERIFY( WandType )  \
	&& WandType ## _IS_GOOD( (WandType *) wnd ) == MagickTrue ) {  \
		RETURN_TRUE;  \
	}  \
	RETURN_FALSE;

#define MW_GET_WAND_VERIFY_TYPE_RET_BOOL( WandType ) {  \
	PRV_START_GET_WAND_VERIFY( WandType )  \
		MW_FETCH_RSRC( WandType, wnd, zvl_pp_args_arr[0] ) == MagickTrue  \
	PRV_END_GET_WAND_VERIFY( WandType );  \
}

#define MW_GET_PixelWand_VERIFY_TYPE_RET_BOOL() {  \
	PRV_START_GET_WAND_VERIFY( PixelWand )  \
		(   MW_FETCH_RSRC( PixelWand,              wnd, zvl_pp_args_arr[0] ) == MagickTrue  \
		 || MW_FETCH_RSRC( PixelIteratorPixelWand, wnd, zvl_pp_args_arr[0] ) == MagickTrue  \
		)  \
	PRV_END_GET_WAND_VERIFY( PixelWand );  \
}

/* ************************************************************************************************************** */

#define PRV_BEGIN_SIMPLE_PHP_FUNC( WandType )  \
	WandType *wnd;  zval *wnd_rsrc_zvl_p;  \
\
	MW_GET_1_ARG( "r", &wnd_rsrc_zvl_p );  \
	MW_GET_POINTER_FROM_RSRC( WandType, wnd, &wnd_rsrc_zvl_p );

/* ************************************************************************************************************** */

/* Calls registered PHP resource destruction function when finally destroyed by PHP; */
#define MW_GET_RSRC_DESTROY_POINTER( ResType ) {  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( ResType );  \
	zend_list_delete( Z_LVAL_P( wnd_rsrc_zvl_p ) );  \
	RETURN_TRUE;  \
}

/* Calls registered PHP PixelWand resource destruction function when finally destroyed by PHP;
   ensures that the destruction of PicelIterator PixelWands is not even attempted
*/
#define MW_DESTROY_ONLY_PixelWand( pxl_wnd, pxl_wnd_rsrc_zvl_pp )  \
	MW_GET_ONLY_PixelWand_POINTER_FROM_RSRC( pxl_wnd, (pxl_wnd_rsrc_zvl_pp) );  \
	zend_list_delete( Z_LVAL_PP( pxl_wnd_rsrc_zvl_pp ) );

/* ************************************************************************************************************** */

#define MW_CHECK_CONSTANT( ConstType, var )  \
	if ( !( PRV_IS_ ## ConstType ( var ) ) ) {  \
		MW_SPIT_FATAL_ERR( "the parameter sent did not correspond to the required " #ConstType " type" ); return;  \
	}

#define MW_CHECK_PARAM_STR_LEN( len_var )  \
	if( (len_var) < 1 ) { MW_SPIT_FATAL_ERR( "Parameter cannot be an empty string" ); return; }

#define MW_CHECK_PARAM_STR_LEN_EX( comparison )  \
	if( (comparison) ) { MW_SPIT_FATAL_ERR( "Parameter(s) cannot be an empty string" ); return; }

/* ************************************************************************************************************** */

#define MW_GET_WAND_RET_DOUBLE( WandType, FunctionName ) {  \
	double ret_double;  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( WandType );  \
	ret_double = (double) (FunctionName ( wnd ));  \
	if ( wnd->exception.severity == UndefinedException ) {  \
		RETURN_DOUBLE( ret_double );  \
	}  \
	RETURN_FALSE;  \
}

#define MW_GET_WAND_RET_LONG( WandType, FunctionName ) {  \
	long ret_long;  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( WandType );  \
	ret_long = (long) (FunctionName ( wnd ));  \
	if ( wnd->exception.severity == UndefinedException ) {  \
		RETURN_LONG( ret_long );  \
	}  \
	RETURN_FALSE;  \
}

/* Arguments: a MagickWand, and a function call
   Sets up returned string for MW_GET_WAND_RETVAL_STRING() and MW_FUNC_RETVAL_STRING_L()
   below; also returs FALSE if the "FunctionCall" returns an empty string, AND the wand contains
   an error. This is potentially problematic: what is the error occurred before, and not as a
   result of this "FunctionCall"? This macro would still cause the PHP function to return an
   "error" (or rather, notice of one, i.e., FALSE)  :(
*/
#define PRV_INIT_RET_STR( wnd, FunctionCall, _STR_MACRO )  \
{  \
	char *ret_str;  \
	ret_str = (char *) (FunctionCall);  \
\
	if ( ret_str == (char *) NULL || *ret_str == '\0' ) {  \
		if ( (wnd)->exception.severity == UndefinedException ) {  \
			RETVAL_MW_EMPTY_STRING();  \
		}  \
		else {  \
			RETVAL_FALSE;  \
		}  \
	}  \
	else {  \
		RETVAL ## _STR_MACRO;  \
	}  \
	MW_FREE_MAGICK_MEM( char *, ret_str );  \
}

/* Arguments: a function's name (the function MUST be one that requires a MagickWand as its ONLY
			  argument
   This macro sets up a PHP fuction which accepts 1 *Wand resource, retrieves the *Wand
   from the resource, calls the function indicated by "FunctionName" (which must return a char*)
   with said MagickWand as its only argument, and returns to the PHP script either the string
   (char*) or FALSE, if the MagickWand contained an error. See above, (re: PRV_INIT_RET_STR)
   for associated dilema.
*/
#define MW_GET_WAND_RETVAL_STRING( WandType, FunctionName ) {  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( WandType );  \
	PRV_INIT_RET_STR( wnd, (FunctionName ( wnd )), _STRING( ret_str, 1 ) );  \
}

#define MW_FUNC_RETVAL_STRING_L( wnd, FunctionCall, str_len ) {  \
	PRV_INIT_RET_STR( wnd, FunctionCall, _STRINGL( ret_str, (size_t) str_len, 1 ) );  \
}

#define MW_BOOL_FUNC_RETVAL_BOOL( FunctionCall )	if( (FunctionCall) == MagickTrue ) { RETVAL_TRUE; } else { RETVAL_FALSE; }

#define MW_GET_WAND_RETVAL_FUNC_BOOL( WandType, FunctionName ) {  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( WandType );  \
	MW_BOOL_FUNC_RETVAL_BOOL( (FunctionName ( wnd )) );  \
}

#define MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( WandType, FunctionName ) {  \
	WandType *wnd;  zval *wnd_rsrc_zvl_p;  char *str;  int str_len;  \
\
	MW_GET_3_ARG( "rs", &wnd_rsrc_zvl_p, &str, &str_len );  \
\
	MW_CHECK_PARAM_STR_LEN( str_len );  \
	MW_GET_POINTER_FROM_RSRC( WandType, wnd, &wnd_rsrc_zvl_p );  \
	MW_BOOL_FUNC_RETVAL_BOOL( (FunctionName ( wnd, str )) );  \
}

/* ************************************************************************************************************** */

#define MW_GET_MIME_TYPE( GetFormatFunc )  \
{  \
	MagickWand *mgck_wnd;  \
	zval *mgck_wnd_rsrc_zvl_p;  \
	char *format, *mime_type;  \
\
	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );  \
	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );  \
\
	format = (char *) (GetFormatFunc ( mgck_wnd ));  \
\
	if ( format == (char *) NULL || *format == '\0' || *format == '*' ) {  \
		MW_API_FUNC_FAIL_CHECK_WAND_ERROR( mgck_wnd, MagickGetException, "unable to retrieve image's format" );  \
	}  \
	else {  \
		mime_type = (char *) MagickToMime( format );  \
\
		if ( mime_type == (char *) NULL || *mime_type == '\0' ) {  \
			zend_error( MW_E_ERROR, "%s(): a mime-type for the specified image format could not be found",  \
									get_active_function_name(TSRMLS_C) );  \
		}  \
		else {  \
			RETVAL_STRING( mime_type, 1 );  \
		}  \
\
		MW_FREE_MAGICK_MEM( char *, mime_type );  \
	}  \
	MW_FREE_MAGICK_MEM( char *, format );  \
}

/* ************************************************************************************************************** */

#define MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element )  \
	for ( zend_hash_internal_pointer_reset_ex( Z_ARRVAL_P( zvl_arr ), &pos );  \
		  zend_hash_get_current_data_ex( Z_ARRVAL_P( zvl_arr ), (void **) &zvl_pp_element, &pos ) == SUCCESS;  \
		  zend_hash_move_forward_ex( Z_ARRVAL_P( zvl_arr ), &pos ) )

/* ************************************************************************************************************** */

#define MW_RET_STRING_ARR( num_strs, FunctionCall ) {  \
	char **ret_str_arr = (char **) NULL;  \
	unsigned long i;  \
\
	ret_str_arr = (char **) FunctionCall;  \
\
	array_init( return_value );  \
\
	if ( (num_strs) > 0 && ret_str_arr != (char **) NULL ) {  \
		for ( i = 0; i < (num_strs); i++ ) {  \
			if ( add_next_index_string( return_value, ret_str_arr[i], 1 ) == FAILURE ) {  \
				MW_SPIT_FATAL_ERR( "error adding a value to the return array" );  \
				break;  \
			}  \
		}  \
	}  \
	MW_FREE_MAGICK_MEM( char **, ret_str_arr );  \
}

#define MW_RETURN_QUERY_STRING_ARR( FunctionName ) {  \
	char *pttrn;  \
	int pttrn_len;  \
	unsigned long num_strs = 0;  \
	MW_GET_2_ARG( "s", &pttrn, &pttrn_len );  \
	MW_CHECK_PARAM_STR_LEN( pttrn_len );  \
	MW_RET_STRING_ARR( num_strs, (FunctionName ( pttrn, &num_strs ) ) );  \
}

/* ************************************************************************************************************** */

#define MW_GET_WAND_RET_DOUBLE_ARR( WandType, FunctionName )  \
{  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( WandType );  \
	{  \
		unsigned long num_elements = 0, i;  \
		double *double_arr = (double *) NULL;  \
\
		double_arr = (double *) (FunctionName ( wnd, &num_elements ));  \
\
		array_init( return_value );  \
\
		if ( (num_elements) > 0 && double_arr != (double *) NULL ) {  \
			for ( i = 0; i < (num_elements); i++ ) {  \
				if ( add_next_index_double( return_value, double_arr[i] ) == FAILURE ) {  \
					MW_SPIT_FATAL_ERR( "error adding a value to the return array" );  \
					break;  \
				}  \
			}  \
		}  \
		MW_FREE_MAGICK_MEM( double *, double_arr );  \
	}  \
}

#define MW_RET_RESOURCE_ARR( WandType, wnd_arr, num_idxs, TypeForZendStorage ) {  \
	unsigned long i;    int wnd_rsrc_id;  \
	array_init( return_value );  \
\
	for ( i = 0; i < num_idxs; i++) {  \
		if (   wnd_arr[i] == (WandType *) NULL  \
			|| MW_ZEND_REGISTER_RESOURCE( TypeForZendStorage, wnd_arr[i], (zval *) NULL, &wnd_rsrc_id ) == MagickFalse  \
			|| add_next_index_resource( return_value, wnd_rsrc_id ) == FAILURE )  \
		{  \
			zend_error( MW_E_ERROR,  \
						"%s(): the " #WandType " at index %d of %d of return array could not be registered",  \
						get_active_function_name(TSRMLS_C), i, num_idxs );  \
			return;  \
		}  \
	}  \
}
/*
  zval **zvl_pp_element;    HashPosition pos;  
  MW_ITERATE_OVER_PHP_ARRAY( pos, return_value, zvl_pp_element ) {  \
	  zend_list_delete( Z_LVAL_PP( zvl_pp_element ) );  \
  }  \
*/

#define PRV_CHECK_ERR_RET_RESOURCE_ARR( WandType, FunctionCall, num_idxs, TypeForZendStorage, err_wnd ) {  \
	WandType **wnd_arr = (WandType **) NULL;  \
	wnd_arr = (WandType **) FunctionCall;  \
\
	if ( wnd_arr == (WandType **) NULL ) {  \
		RETURN_FALSE;  \
	}  \
	else {  \
		if ( (err_wnd)->exception.severity != UndefinedException ) {  \
			MW_FREE_MAGICK_MEM( WandType **, wnd_arr );  \
			RETURN_FALSE;  \
		}  \
		MW_RET_RESOURCE_ARR( WandType, wnd_arr, num_idxs, TypeForZendStorage );  \
	}  \
}

#define MW_CHECK_ERR_RET_RESOURCE_ARR( WandType, FunctionCall, num_idxs, err_wnd )  \
	PRV_CHECK_ERR_RET_RESOURCE_ARR( WandType, FunctionCall, num_idxs, WandType, err_wnd );

#define MW_CHECK_ERR_RET_PixelIteratorPixelWand_ARR( FunctionName ) {  \
	PixelIterator *pxl_iter;    zval *pxl_iter_rsrc_zvl_p;  \
	unsigned long num_wnds;  \
	MW_GET_1_ARG( "r", &pxl_iter_rsrc_zvl_p );  \
	MW_GET_POINTER_FROM_RSRC( PixelIterator, pxl_iter, &pxl_iter_rsrc_zvl_p );  \
	PRV_CHECK_ERR_RET_RESOURCE_ARR( PixelWand, (FunctionName ( pxl_iter, &num_wnds )), num_wnds, PixelIteratorPixelWand, pxl_iter );  \
}

/* ************************************************************************************************************** */

#define MW_SET_1_RET_ARR_VAL( ZendFuncCall )  \
	if ( (ZendFuncCall) == FAILURE ) {  \
		MW_SPIT_FATAL_ERR( "error adding a value to the array to be returned" ); return;  \
	}

#define MW_SET_2_RET_ARR_VALS( ZendFuncCall_1, ZendFuncCall_2 )  \
	if ( (ZendFuncCall_1) == FAILURE || (ZendFuncCall_2) == FAILURE ) {  \
		MW_SPIT_FATAL_ERR( "error adding a value to the array to be returned" ); return;  \
	}

#define MW_CHK_BOOL_RET_2_IDX_DBL_ARR( WandFuncCall, val_0, val_1 )  \
	if ( WandFuncCall == MagickTrue ) {  \
		array_init( return_value );  \
		MW_SET_2_RET_ARR_VALS( add_index_double( return_value, 0, val_0 ),  \
							   add_index_double( return_value, 1, val_1 ) );  \
	}  \
	else {  RETURN_FALSE;  }

#define MW_RET_ARR_ADD_NEXT_IDX_EMPTY_STR()		MW_SET_1_RET_ARR_VAL( add_next_index_stringl( return_value, "", 0, 1 ) );
#define MW_RET_ARR_ADD_NEXT_IDX_FALSE()			MW_SET_1_RET_ARR_VAL( add_next_index_bool( return_value, FALSE ) );

/* ************************************************************************************************************** */

#define MW_INIT_FONT_METRIC_ARRAY( font_metric_arr )  \
	MagickWand *mgck_wnd;  DrawingWand *drw_wnd;  zval *mgck_wnd_rsrc_zvl_p, *drw_wnd_rsrc_zvl_p;  \
	char *txt;  int txt_len;  double *font_metric_arr = (double *) NULL;  \
	zend_bool multiline = FALSE;  \
\
	MW_GET_5_ARG( "rrs|b", &mgck_wnd_rsrc_zvl_p, &drw_wnd_rsrc_zvl_p, &txt, &txt_len, &multiline );  \
\
	MW_CHECK_PARAM_STR_LEN( txt_len );  \
	MW_GET_POINTER_FROM_RSRC( MagickWand,  mgck_wnd, &mgck_wnd_rsrc_zvl_p );  \
	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd,  &drw_wnd_rsrc_zvl_p  );  \
\
	if ( multiline == FALSE ) {  \
		font_metric_arr = (double *) MagickQueryFontMetrics( mgck_wnd, drw_wnd, txt );  \
	}  \
	else {  \
		font_metric_arr = (double *) MagickQueryMultilineFontMetrics( mgck_wnd, drw_wnd, txt );  \
	}

#define MW_RETURN_FONT_METRIC( idx ) {  \
	MW_INIT_FONT_METRIC_ARRAY( font_metric_arr );  \
\
	if( font_metric_arr == (double *) NULL ) {  \
		RETURN_FALSE;  \
	}  \
	else {  \
		RETVAL_DOUBLE( font_metric_arr[idx] );  \
		font_metric_arr = (double *) MagickRelinquishMemory( font_metric_arr );  \
	}  \
}

/* ************************************************************************************************************** */

#define DrawingWand_GET_EXCEPTION(   wnd, severity_p )  DrawGetException(          wnd, severity_p )
#define MagickWand_GET_EXCEPTION(    wnd, severity_p )  MagickGetException(        wnd, severity_p )
#define PixelWand_GET_EXCEPTION(     wnd, severity_p )  PixelGetException(         wnd, severity_p )
#define PixelIterator_GET_EXCEPTION( wnd, severity_p )  PixelGetIteratorException( wnd, severity_p )

#define PRV_EXCEPT_FUNC_DECLARES( WandType )  \
	WandType *wnd;  zval *wnd_rsrc_zvl_p;  \
\
	MW_GET_1_ARG( "r", &wnd_rsrc_zvl_p );  \
\
	if (   MW_FETCH_RSRC( WandType, wnd, &wnd_rsrc_zvl_p ) == MagickFalse  \
		|| WandType ## _IS_GOOD( wnd ) == MagickFalse ) {  \
		MW_SPIT_FATAL_ERR( "function requires a " #WandType " resource" );  \
		return;  \
	}

#define RETURN_MW_NO_ERR_ARRAY()  \
	array_init( return_value );  \
	MW_SET_2_RET_ARR_VALS( add_next_index_stringl( return_value, "", 0, 1 ),  \
						   add_next_index_long( return_value, (long) UndefinedException ) );  \
	return;

#define PRV_GET_EXCEPTION_RET_ZEND_TYPE( WandType, wnd, ZendType, err_str )  \
	char *err_str;  \
	ExceptionType severity;  \
	if ( (wnd)->exception.severity == UndefinedException ) { RETURN ## ZendType; }  \
	err_str = (char *) WandType ## _GET_EXCEPTION( (wnd), &severity );

#define MW_GET_WAND_RET_EXCEPTION_ARR( WandType ) {  \
	PRV_EXCEPT_FUNC_DECLARES( WandType );  \
	{  \
		PRV_GET_EXCEPTION_RET_ZEND_TYPE( WandType, wnd, _MW_NO_ERR_ARRAY(), err_str );  \
		array_init( return_value );  \
		MW_SET_2_RET_ARR_VALS( add_next_index_string( return_value, err_str, 1 ),  \
							   add_next_index_long( return_value, (long) severity ) );   \
		MW_FREE_MAGICK_MEM( char *, err_str );  \
	}  \
}

#define MW_GET_WAND_RET_EXCEPTION_STR( WandType ) {  \
	PRV_EXCEPT_FUNC_DECLARES( WandType );  \
	{  \
		PRV_GET_EXCEPTION_RET_ZEND_TYPE( WandType, wnd, _MW_EMPTY_STRING(), err_str );  \
		RETVAL_STRING( err_str, 1 );  \
		MW_FREE_MAGICK_MEM( char *, err_str );  \
	}  \
}

#define MW_GET_WAND_RET_EXCEPTION_TYPE( WandType ) {  \
	PRV_EXCEPT_FUNC_DECLARES( WandType );  \
	RETURN_LONG( (long) (wnd->exception.severity) );  \
}

/* ************************************************************************************************************** */

#define PRV_SETUP_WAND_ERR_IF_COND( WandType, resource, ZendType )  \
	WandType *wnd = (WandType *) resource;  \
	MW_CHECK_IF_GOOD_SPIT_ERR( WandType, wnd );  \
	if ( (wnd)->exception.severity == UndefinedException ) { RETURN ## ZendType; }

#define PRV_ERR_ARR_SETUP( WandType, resource ) {  \
	char *err_str;  \
	ExceptionType severity;  \
	PRV_SETUP_WAND_ERR_IF_COND( WandType, resource, _MW_NO_ERR_ARRAY() );  \
	err_str = (char *) WandType ## _GET_EXCEPTION( wnd, &(severity) );  \
	array_init( return_value );  \
	MW_SET_2_RET_ARR_VALS( add_next_index_string( return_value, err_str, 1 ),  \
						   add_next_index_long( return_value, (long) severity ) );  \
	MW_FREE_MAGICK_MEM( char *, err_str );  \
}

#define PRV_ERR_STR_SETUP( WandType, resource ) {  \
	char *err_str;  \
	ExceptionType severity;  \
	PRV_SETUP_WAND_ERR_IF_COND( WandType, resource, _MW_EMPTY_STRING() );  \
	err_str = (char *) WandType ## _GET_EXCEPTION( wnd, &(severity) );  \
	RETVAL_STRING( err_str, 1 );  \
	MW_FREE_MAGICK_MEM( char *, err_str );  \
}

#define PRV_ERR_TYPE_SETUP( WandType, resource ) {  \
	PRV_SETUP_WAND_ERR_IF_COND( WandType, resource, _LONG((long) UndefinedException) );  \
	RETURN_LONG( (long) ((wnd)->exception.severity) );  \
}

#define PRV_ERR_BOOL_SETUP( WandType, resource ) {  \
	PRV_SETUP_WAND_ERR_IF_COND( WandType, resource, _FALSE );  \
	RETURN_TRUE;  \
}

#define PRV_DET_RSRC_TYPE_SET_ERR_STR_OR_TYPE( ErrorReturnMethod ) {  \
	zval *wnd_rsrc_zvl_p;  \
	int id, le_sent_rsrc_type = -1;  \
	void *resource;  \
\
	MW_GET_1_ARG( "r", &wnd_rsrc_zvl_p );  \
\
	id = Z_LVAL_P( wnd_rsrc_zvl_p );  \
\
	resource = (void *) zend_list_find( id, &le_sent_rsrc_type );  \
\
	if ( le_sent_rsrc_type == -1 || resource == (void *) NULL ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): %d is not a valid MagickWand module resource (i.e. the "  \
						"resource sent to this function must be a DrawingWand, "  \
						"MagickWand, PixelWand, or PixelIterator resource)",  \
					get_active_function_name(TSRMLS_C), id );  \
		return;  \
	}  \
\
	if ( le_sent_rsrc_type == le_MagickWand ) {  \
		ErrorReturnMethod ## SETUP( MagickWand, resource );  \
	}  \
	else {  \
		if ( le_sent_rsrc_type == le_DrawingWand ) {  \
			ErrorReturnMethod ## SETUP( DrawingWand, resource );  \
		}  \
		else {  \
			if ( le_sent_rsrc_type == le_PixelIteratorPixelWand || le_sent_rsrc_type == le_PixelWand ) {  \
				ErrorReturnMethod ## SETUP( PixelWand, resource );  \
			}  \
			else {  \
				if ( le_sent_rsrc_type == le_PixelIterator ) {  \
					ErrorReturnMethod ## SETUP( PixelIterator, resource );  \
				}  \
				else {  \
					zend_error( MW_E_ERROR,  \
								"%s(): %d is not a valid MagickWand module resource (i.e. the "  \
									"resource sent to this function must be a DrawingWand, "  \
									"MagickWand, PixelWand, or PixelIterator resource)",  \
								get_active_function_name(TSRMLS_C), id );  \
					return;  \
				}  \
			}  \
		}  \
	}  \
}

#define MW_DETERMINE_RSRC_TYPE_SET_ERROR_ARR()  PRV_DET_RSRC_TYPE_SET_ERR_STR_OR_TYPE( PRV_ERR_ARR_ );
#define MW_DETERMINE_RSRC_TYPE_SET_ERROR_STR()  PRV_DET_RSRC_TYPE_SET_ERR_STR_OR_TYPE( PRV_ERR_STR_ );
#define MW_DETERMINE_RSRC_TYPE_SET_ERR_TYPE()   PRV_DET_RSRC_TYPE_SET_ERR_STR_OR_TYPE( PRV_ERR_TYPE_ );
#define MW_DETERMINE_RSRC_TYPE_SET_ERR_BOOL()   PRV_DET_RSRC_TYPE_SET_ERR_STR_OR_TYPE( PRV_ERR_BOOL_ );

/* ************************************************************************************************************** */

#define MW_MK_MGCK_BOOL( var )		( (var) == TRUE ? MagickTrue : MagickFalse )

/* ************************************************************************************************************** */

#define MW_GET_WAND_DRAW_POINTS( min_num_args, FunctionName ) {  \
	DrawingWand *drw_wnd;  \
	PointInfo *coord_arr;  \
	zval ***zvl_pp_args_arr;  \
	int arg_count = ZEND_NUM_ARGS(), i, j = 0;  \
	unsigned long num_coords;  \
\
	if ( arg_count < min_num_args ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): error in function call: function requires at least " #min_num_args  \
					" parameters -- a DrawingWand resource and at least %d integer ordinate values)",  \
					get_active_function_name(TSRMLS_C), (min_num_args - 1) );  \
		return;  \
	}  \
\
	MW_ARR_ECALLOC( zval **, zvl_pp_args_arr, arg_count );  \
\
	if ( zend_get_parameters_array_ex( arg_count, zvl_pp_args_arr ) == FAILURE ) {  \
		MW_SPIT_FATAL_ERR( "unknown error occurred in function call" );  \
		efree( zvl_pp_args_arr );  \
		return;  \
	}  \
\
	if (   Z_TYPE_PP( zvl_pp_args_arr[0] ) != IS_RESOURCE  \
		|| MW_FETCH_RSRC( DrawingWand, drw_wnd, zvl_pp_args_arr[0] ) == MagickFalse  \
		|| IsDrawingWand( drw_wnd ) == MagickFalse ) {  \
		MW_SPIT_FATAL_ERR( "function requires a DrawingWand resource as its first argument" );  \
		efree( zvl_pp_args_arr );  \
		return;  \
	}  \
	DrawClearException( drw_wnd ); \
\
	num_coords = (unsigned long) ((arg_count - 1) >> 1);  \
\
	if ( (arg_count - 1 - num_coords) != num_coords ) {  \
		MW_SPIT_FATAL_ERR( "function requires a DrawingWand resource, and an EVEN-length list of"  \
						   "numbers (the number pairs represent points in 2D co-ordinate space)" );  \
		return;  \
	}  \
\
	coord_arr = (PointInfo *) ecalloc( (size_t) num_coords, sizeof(PointInfo) );  \
	if ( coord_arr == (PointInfo *) NULL ) {  \
		MW_SPIT_FATAL_ERR( "cannot allocate memory for PointInfo array (PointInfo*)" );  \
		efree( zvl_pp_args_arr );  \
		return;  \
	}  \
\
	for ( i = 1; i < arg_count; i++ ) {  \
		convert_to_double_ex( zvl_pp_args_arr[i] );  \
		(coord_arr[j  ]).x = Z_DVAL_PP( zvl_pp_args_arr[i] );  \
		i++;  \
		convert_to_double_ex( zvl_pp_args_arr[i] );  \
		(coord_arr[j++]).y = Z_DVAL_PP( zvl_pp_args_arr[i] );  \
	}  \
\
	FunctionName ( drw_wnd, num_coords, coord_arr );  \
	efree( zvl_pp_args_arr );  \
	efree( coord_arr );  \
}

#define MW_GET_WAND_AND_ARRAY_DRAW_POINTS( min_num_ords, FunctionName ) {  \
	DrawingWand *drw_wnd;  \
	PointInfo *coord_arr;  \
	zval *drw_wnd_rsrc_zvl_p, *zvl_arr, **zvl_pp_element;  \
	int num_ords, i = 0;  \
	unsigned long num_coords;  \
	HashPosition pos;  \
\
	MW_GET_2_ARG( "ra", &drw_wnd_rsrc_zvl_p, &zvl_arr );  \
\
	num_ords = zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );  \
\
	if ( num_ords < min_num_ords ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): function requires an array containing at least " #min_num_ords " ordinate values",  \
					get_active_function_name(TSRMLS_C) );  \
		return;  \
	}  \
\
	num_coords = (unsigned long) (num_ords >> 1);  \
\
	if ( (num_ords - num_coords) != num_coords ) {  \
		MW_SPIT_FATAL_ERR( "co-ordinate array parameter must contain an even number of ordinates" );  \
		return;  \
	}  \
\
	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );  \
\
	MW_ARR_ECALLOC( PointInfo, coord_arr, num_coords );  \
\
	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element ) {  \
		convert_to_double_ex( zvl_pp_element );  \
		(coord_arr[i]).x = Z_DVAL_PP( zvl_pp_element );  \
\
		zend_hash_move_forward_ex( Z_ARRVAL_P( zvl_arr ), &pos );  \
\
		if ( zend_hash_get_current_data_ex( Z_ARRVAL_P( zvl_arr ), (void **) &zvl_pp_element, &pos ) == FAILURE ) {  \
			efree( coord_arr );  \
			MW_SPIT_FATAL_ERR( "error iterating through PHP co-ordinate array parameter" );  \
			return;  \
		}  \
		convert_to_double_ex( zvl_pp_element );  \
		(coord_arr[i++]).y = Z_DVAL_PP( zvl_pp_element );  \
	}  \
\
	FunctionName ( drw_wnd, num_coords, coord_arr );  \
	efree( coord_arr );  \
}

/* ************************************************************************************************************** */

#define PRV_GET_WAND_SETUP_PIXELWAND( WandType )  \
	PixelWand *pxl_wnd;  \
	PRV_BEGIN_SIMPLE_PHP_FUNC( WandType );  \
	pxl_wnd = (PixelWand *) NewPixelWand();

#define MW_GET_DRAWINGWAND_DO_VOID_FUNC_RET_PIXELWAND( FunctionName ) {  \
	PRV_GET_WAND_SETUP_PIXELWAND( DrawingWand );  \
	FunctionName ( wnd, pxl_wnd );  \
	MW_SET_RET_RSRC( PixelWand, pxl_wnd );  \
}

#define MW_GET_MAGICKWAND_DO_BOOL_FUNC_RET_PIXELWAND( FunctionName ) {  \
	PRV_GET_WAND_SETUP_PIXELWAND( MagickWand );  \
	if ( FunctionName( wnd, pxl_wnd ) == MagickTrue ) {  \
		MW_SET_RET_RSRC( PixelWand, pxl_wnd );  \
	}  \
	else {  \
		pxl_wnd = DestroyPixelWand( pxl_wnd );  \
		RETURN_FALSE;  \
	}  \
}

/* ************************************************************************************************************** */

#define MW_STD_STREAM_OPTS		(IGNORE_PATH | IGNORE_URL | ENFORCE_SAFE_MODE | REPORT_ERRORS)

#define MW_IS_R_STRING	"readable"
#define MW_IS_W_STRING	"writeable"

#define MW_IS_R		"r"
#define MW_IS_W		"a"
#define MW_IS_A		"a"

/*  I don't like the following workaround, but I have not yet found a way other than this,
	to check if PHP can access a file, while enforcing safemode restrictions.
*/
#define PRV_CHECK_FILE_STATUS( file_name, WhatStat ) {  \
	php_stream *stream;  \
	stream = php_stream_open_wrapper( file_name, WhatStat "b", MW_STD_STREAM_OPTS, (char **) NULL );  \
	MW_NULL_PNTER_ERR_CHECK(  \
		php_stream,  \
		stream,  \
		"the filename parameter sent to this function was not " WhatStat ## _STRING " by this script" );  \
	php_stream_close( stream );  \
}

#define MW_FILE_IS_READABLE(  file_name )  PRV_CHECK_FILE_STATUS( file_name, MW_IS_R );
#define MW_FILE_IS_WRITEABLE( file_name )  PRV_CHECK_FILE_STATUS( file_name, MW_IS_W );

/* ************************************************************************************************************** */
/* If you have more images in a MagickWand than 10,000, and you still have memory, more power to you,
   but I figure that is at best unlikely, and at worst, bad programming, since setting up a loop and
   going through the images in other ways is a better, faster and less memory intensive idea
*/
#define MW_SET_ZERO_FILLED_WIDTH( mgck_wnd, field_width )  \
	field_width = (int) MagickGetNumberImages( mgck_wnd );  \
	if ( field_width < 10 ) { field_width = 1; }  \
	else {  \
		if ( field_width < 100 ) { field_width = 2; }  \
		else {  \
			if ( field_width < 1000 ) { field_width = 3; }  \
			else {  \
				if ( field_width < 10000 ) { field_width = 4; }  \
				else {  \
					field_width = (int) ( 1.0 + log10( (double) field_width ) );  \
				}  \
			}  \
		}  \
	}  \
	MW_DEBUG_NOTICE_EX( "field_width = %d", field_width );

/* ************************************************************************************************************** */

#define MW_GET_WAND_SET_NORMALIZED_COLOR( WandType, FunctionName ) {  \
	WandType *wnd;  zval *wnd_rsrc_zvl_p;  double norm_color;  \
\
	MW_GET_2_ARG( "rd", &wnd_rsrc_zvl_p, &norm_color );  \
\
	if ( norm_color < 0.0 || norm_color > 1.0 ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): value of color/alpha/opacity argument (%f) was invalid. "  \
					"Value must be normalized to \"0 <= color_val <= 1\"",  \
					get_active_function_name( TSRMLS_C ),  \
					norm_color );  \
		return;  \
	}  \
	MW_GET_POINTER_FROM_RSRC( WandType, wnd, &wnd_rsrc_zvl_p );  \
	FunctionName ( wnd, norm_color );  \
}

/* ************************************************************************************************************** */

#define MW_GET_PIXELWAND_SET_QUANTUM_COLOR( FunctionName ) {  \
	PixelWand *pxl_wnd;  zval *pxl_wnd_rsrc_zvl_p;  double color_qntm;  \
\
	MW_GET_2_ARG( "rd", &pxl_wnd_rsrc_zvl_p, &color_qntm );  \
	if ( color_qntm < 0.0 || color_qntm > MW_MaxRGB ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): value of Quantum color/alpha/opacity argument (%0.0f) was invalid. "  \
					"Quantum color values must match \"0 <= color_val <= %0.0f\"",  \
					get_active_function_name( TSRMLS_C ), color_qntm, MW_MaxRGB );  \
		return;  \
	}  \
	MW_GET_POINTER_FROM_RSRC( PixelWand, pxl_wnd, &pxl_wnd_rsrc_zvl_p );  \
	FunctionName ( pxl_wnd, (Quantum) color_qntm );  \
}

/* ************************************************************************************************************** */

#define PRV_GET_WAND_AND_ONE_NUM( WandType, argtype, specstring )  \
	WandType *wnd;  zval *wnd_rsrc_zvl_p;  \
	argtype arg;  \
	MW_GET_2_ARG( "r" specstring, &wnd_rsrc_zvl_p, &arg );  \
	MW_GET_POINTER_FROM_RSRC( WandType, wnd, &wnd_rsrc_zvl_p );

#define MW_GET_DRAWINGWAND_SET_ENUM( EnumType, FunctionName ) {  \
	PRV_GET_WAND_AND_ONE_NUM( DrawingWand, long, "l" );  \
	MW_CHECK_CONSTANT( EnumType, arg );  \
	FunctionName ( wnd, (EnumType) arg );  \
}

#define MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( EnumType, FunctionName ) {  \
	PRV_GET_WAND_AND_ONE_NUM( MagickWand, long, "l" );  \
	MW_CHECK_CONSTANT( EnumType, arg );  \
	MW_BOOL_FUNC_RETVAL_BOOL( FunctionName ( wnd, (EnumType) arg ) );  \
}

#define MW_GET_DRAWINGWAND_SET_DOUBLE( FunctionName ) {  \
	PRV_GET_WAND_AND_ONE_NUM( DrawingWand, double, "d" );  \
	FunctionName ( wnd, arg );  \
}

#define MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( FunctionName ) {  \
	PRV_GET_WAND_AND_ONE_NUM( MagickWand, double, "d" );  \
	MW_BOOL_FUNC_RETVAL_BOOL( FunctionName ( wnd, arg ) );  \
}

/* ************************************************************************************************************** */

#define MW_CHECK_VALUE_RANGE( var, min_val, max_val )  \
	if ( var < min_val || var > max_val ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): " #var " value (%d) was invalid. "  \
					"Value must match \"%d <= " #var " <= %d\"",  \
					get_active_function_name( TSRMLS_C ),  \
					var, min_val, max_val );  \
		return;  \
	}


/* ************************************************************************************************************** */


#define MW_GET_ARGS_ARRAY_EX( arg_count, arg_count_fail_check, zvl_pp_args_arr, WandType, wnd, arg_desc_str_literal )  \
{  \
	arg_count = ZEND_NUM_ARGS();  \
	if ( (arg_count_fail_check) ) {  \
		MW_SPIT_FATAL_ERR(	"%s(): error in function call: function requires " arg_desc_str_literal );  \
		return;  \
	}  \
	MW_ARR_ECALLOC( zval **, zvl_pp_args_arr, arg_count );  \
\
	if ( zend_get_parameters_array_ex( arg_count, zvl_pp_args_arr ) == FAILURE ) {  \
		MW_SPIT_FATAL_ERR( "unknown error occurred in function call" );  \
		efree( zvl_pp_args_arr );  \
		return;  \
	}  \
\
	if (   Z_TYPE_PP( zvl_pp_args_arr[0] ) != IS_RESOURCE  \
		|| MW_FETCH_RSRC( WandType, wnd, zvl_pp_args_arr[0] ) == MagickFalse  \
		|| WandType ## _IS_GOOD( wnd ) == MagickFalse )  \
	{  \
		MW_SPIT_FATAL_ERR( "function requires a " #WandType " resource as its first argument" );  \
		efree( zvl_pp_args_arr );  \
		return;  \
	}  \
	WandType ## _CLEAR_EXCEPTION( wnd );  \
}


/* ************************************************************************************************************** */


#define MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, idx, arg_num, pxl_wnd, is_script_pxl_wnd )  \
{  \
	if ( Z_TYPE_PP( zvl_pp_args_arr[idx] ) == IS_RESOURCE ) {  \
\
		if (  (   MW_FETCH_RSRC( PixelWand, pxl_wnd, zvl_pp_args_arr[idx] ) == MagickFalse  \
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, pxl_wnd, zvl_pp_args_arr[idx] ) == MagickFalse )  \
			|| IsPixelWand( pxl_wnd ) == MagickFalse )  \
		{  \
			MW_SPIT_FATAL_ERR( "invalid resource type as argument #" #arg_num "; a PixelWand resource is required" );  \
			efree( zvl_pp_args_arr );  \
			return;  \
		}  \
		is_script_pxl_wnd = 1;  \
	}  \
	else {  \
		is_script_pxl_wnd = 0;  \
\
		pxl_wnd = (PixelWand *) NewPixelWand();  \
\
		if ( pxl_wnd == (PixelWand *) NULL ) {  \
			MW_SPIT_FATAL_ERR( "unable to create necessary PixelWand" );  \
			efree( zvl_pp_args_arr );  \
			return;  \
		}  \
		convert_to_string_ex( zvl_pp_args_arr[idx] );  \
\
		if ( Z_STRLEN_PP( zvl_pp_args_arr[idx] ) > 0 ) {  \
\
			if ( PixelSetColor( pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[idx] ) ) == MagickFalse ) {  \
\
				MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	pxl_wnd, PixelGetException,  \
													"could not set PixelWand to desired fill color"  \
				);  \
				pxl_wnd = DestroyPixelWand( pxl_wnd );  \
				efree( zvl_pp_args_arr );  \
				return;  \
			}  \
		}  \
	}  \
}

/* ************************************************************************************************************** */

#define MW_CHECK_FOR_WAND_FORMAT( mgck_wnd, FunctionNameStrConst )  \
{  \
	char *wand_format;  \
	wand_format = (char *) MagickGetFormat( mgck_wnd );  \
\
	if ( wand_format == (char *) NULL || *wand_format == '\0' || *wand_format == '*' ) {  \
		MW_SPIT_FATAL_ERR(	"the MagickWand resource sent to this function did not have an image format set "  \
							"(via MagickSetFormat()); the MagickWand's image format must be set in order "  \
							"for " FunctionNameStrConst "() to continue" );  \
		MW_FREE_MAGICK_MEM( char *, wand_format );  \
		return;  \
	}  \
\
	MW_FREE_MAGICK_MEM( char *, wand_format );  \
}

#define MW_CHECK_FOR_IMAGE_FORMAT( mgck_wnd, img_idx, img_had_format, orig_img_format, FunctionNameStrConst )  \
{  \
	char *wand_format = (char *) NULL, *img_format = (char *) NULL;  \
\
	MW_DEBUG_NOTICE_EX( "Checking image #%d for an image format", img_idx );  \
\
	img_format = (char *) MagickGetImageFormat( mgck_wnd );  \
	if ( img_format == (char *) NULL || *img_format == '\0' || *img_format == '*' ) {  \
\
		MW_DEBUG_NOTICE_EX( "Image #%d has no image format", img_idx );  \
\
		MW_FREE_MAGICK_MEM( char *, img_format );  \
\
		img_had_format = MagickFalse;  \
\
		wand_format = (char *) MagickGetFormat( mgck_wnd );  \
		if ( wand_format == (char *) NULL || *wand_format == '\0' || *wand_format == '*' ) {  \
			zend_error( MW_E_ERROR, "neither the MagickWand resource sent to this function, nor its current "  \
									"active image (index %d) had an image format set (via MagickSetFormat() or "  \
									"MagickSetImageFormat()); the function checks for the current active "  \
									"image's image format, and then for the MagickWand's image format -- "  \
									"one of them must be set in order for " FunctionNameStrConst "() to continue",  \
									get_active_function_name(TSRMLS_C), img_idx );  \
			MW_FREE_MAGICK_MEM( char *, wand_format );  \
			return;  \
		}  \
		else {  \
			MW_DEBUG_NOTICE_EX( "Attempting to set image #%d's image format to the MagickWand's image format", img_idx );  \
\
			if ( MagickSetImageFormat( mgck_wnd, wand_format ) == MagickTrue ) {  \
\
				MW_FREE_MAGICK_MEM( char *, wand_format );  \
\
				MW_DEBUG_NOTICE_EX( "SUCCESS: set image #%d's image format to the MagickWand's image format", img_idx );  \
\
				orig_img_format = (char *) NULL;  \
			}  \
			else {  \
				MW_FREE_MAGICK_MEM( char *, wand_format );  \
\
				MW_DEBUG_NOTICE_EX(	"FAILURE: could not set image #%d's image format to the MagickWand's image format",  \
									img_idx );  \
\
				/* C API cannot set the current image's format to the MagickWand's format: check for/output error, with reason */  \
				MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_2(	mgck_wnd, MagickGetException,  \
														"unable to set the format of the image at index %d to the "  \
														"MagickWand's set image format \"%s\"",  \
														img_idx, wand_format  \
				);  \
				return;  \
			}  \
		}  \
	}  \
	else {  \
		MW_DEBUG_NOTICE_EX( "Image #%d had an image format", img_idx );  \
\
		img_had_format = MagickTrue;  \
\
		orig_img_format = img_format;  \
	}  \
}


/* ************************************************************************************************************** */


#define MW_SPIT_UNKNOWN_API_ERROR()  \
	zend_error(	MW_E_ERROR, "%s(): An unknown C API exception occurred!", get_active_function_name(TSRMLS_C) );


#define PRV_CHECK_WAND_ERROR_SETUP( wnd, ExceptionFuncName, zend_error_FuncCall_1, zend_error_FuncCall_2 )  \
{  \
	if ( wnd->exception.severity == UndefinedException  ) {  \
		MW_SPIT_UNKNOWN_API_ERROR();  \
	}  \
	else {  \
		char *mw_err_str;  \
		ExceptionType mw_severity;  \
\
		mw_err_str = (char *) ExceptionFuncName ( wnd, &mw_severity );  \
		if ( mw_err_str == (char *) NULL || *mw_err_str == '\0' ) {  \
			zend_error_FuncCall_1;  \
		}  \
		else {  \
			zend_error_FuncCall_2;  \
		}  \
		MW_FREE_MAGICK_MEM( char *, mw_err_str );  \
	}  \
}

#define MW_API_FUNC_FAIL_CHECK_WAND_ERROR( wnd, ExceptionFuncName, err_msg_str_const )  \
{  \
	PRV_CHECK_WAND_ERROR_SETUP(	wnd, ExceptionFuncName,  \
								zend_error(	MW_E_ERROR,  \
											"%s(): C API " err_msg_str_const " (reason: unknown)",  \
											get_active_function_name(TSRMLS_C)  \
								),  \
								zend_error(	MW_E_ERROR,  \
											"%s(): C API " err_msg_str_const " (reason: %s)",  \
											get_active_function_name(TSRMLS_C),  \
											mw_err_str  \
								)  \
	);  \
}

#define MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1( wnd, ExceptionFuncName, err_msg_str_const, extra_var_in_str )  \
{  \
	PRV_CHECK_WAND_ERROR_SETUP(	wnd, ExceptionFuncName,  \
								zend_error(	MW_E_ERROR,  \
											"%s(): C API " err_msg_str_const " (reason: unknown)",  \
											get_active_function_name(TSRMLS_C),  \
											extra_var_in_str  \
								),  \
								zend_error(	MW_E_ERROR,  \
											"%s(): C API " err_msg_str_const " (reason: %s)",  \
											get_active_function_name(TSRMLS_C),  \
											extra_var_in_str,  \
											mw_err_str  \
								)  \
	);  \
}

#define MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_2( wnd, ExceptionFuncName, err_msg_str_const, extra_var_in_str_1, extra_var_in_str_2 )  \
{  \
	PRV_CHECK_WAND_ERROR_SETUP(	wnd, ExceptionFuncName,  \
								zend_error(	MW_E_ERROR,  \
											"%s(): C API " err_msg_str_const " (reason: unknown)",  \
											get_active_function_name(TSRMLS_C),  \
											extra_var_in_str_1,  \
											extra_var_in_str_2  \
								),  \
								zend_error(	MW_E_ERROR,  \
											"%s(): C API " err_msg_str_const " (reason: %s)",  \
											get_active_function_name(TSRMLS_C),  \
											extra_var_in_str_1,  \
											extra_var_in_str_2,  \
											mw_err_str  \
								)  \
	);  \
}
