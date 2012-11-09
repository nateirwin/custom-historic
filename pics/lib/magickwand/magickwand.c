
/* MagickWand for PHP main source file; definitions or resources, destructors, and user visible functions

   Author: Ouinnel Watson
   Homepage: 
   Current Version: 0.1.3
   Release Date: 2005-01-11
*/

#include "magickwand_inc.h"

/* ************************************************************************************************************** */

static MagickBooleanType MW_split_filename_on_period( char **filename, const size_t filename_len,
													  char **name_p, const int field_width, char **ext_p,
													  char **tmp_filename_p, size_t *tmp_filename_len_p )
{
	char tmp_char;
	size_t name_len;

	TSRMLS_FETCH();

/*	char *name, *ext, *tmp_filename;

	name = *name_p;
	ext = *ext_p;
	tmp_filename = *tmp_filename_p;
*/
	name_len = (size_t) (filename_len - 4);  /* name_len = length of part of filename before last '.' in a 8.3 filename */
	if ( filename_len > 4 && (*filename)[name_len] == '.' ) {
		*ext_p = &((*filename)[name_len]);  /* set *ext_p to address of '.' before extension in filename */
	}
	else {							/* the filename has no extension which we would mess with, so... */
		*ext_p = "";				/* set extension to an empty string */
		name_len = filename_len;	/* since filename has no extension, set name_len to the length of filename */
	}

	MW_DEBUG_NOTICE_EX( "*ext_p = \"%s\"", *ext_p );
	MW_DEBUG_NOTICE_EX( "name_len = %d", name_len );

	*name_p = (char *) emalloc( name_len + 1 );  /* allocate *name_p (filename before the extension); */
	if ( *name_p == (char *) NULL ) {			/* check for errors */
		MW_SPIT_FATAL_ERR( "out of memory; could not acquire memory for \"*name_p\" char* variable" );
		return MagickFalse;
	}
	/* save character at name_len index in filename (is either '.' or '\0'), then set character at name_len index in filename
	   to '\0' -- i.e., temporarily truncate filename to name_len length, so that strncpy() will end the string reliably there */
	tmp_char = (*filename)[name_len];
	(*filename)[name_len] = '\0';

	*name_p = strncpy( *name_p, *filename, name_len );  /* *name_p = start of filename to char before last '.', or entire filename (if no '.') */
	(*name_p)[name_len] = '\0';

	MW_DEBUG_NOTICE_EX( "*name_p = \"%s\"", *name_p );

	(*filename)[name_len] = tmp_char;  /* set the value of the character at filename's name_len'th index back to original value */

	/* set length of potential new image filenames to length of filename, plus size of field_width, plus length of underscore (1) */
	*tmp_filename_len_p = filename_len + (size_t) field_width + 2;

	MW_DEBUG_NOTICE_EX( "*tmp_filename_len_p = %d", *tmp_filename_len_p );

	/* allocate *tmp_filename_p (variable to hold new filename of images that do not have a filename); the "+1" accomodates the '\0' */
	*tmp_filename_p = (char *) emalloc( *tmp_filename_len_p + 1 );
	if ( *tmp_filename_p == (char *) NULL ) {
		MW_SPIT_FATAL_ERR( "out of memory; could not acquire memory for \"*tmp_filename_p\" char* variable" );
		efree( *name_p );
		return MagickFalse;
	}
	(*tmp_filename_p)[ *tmp_filename_len_p ] = '\0';
	MW_DEBUG_NOTICE( "MW_split_filename_on_period() function completed successfully" );

	return MagickTrue;
}

/* ************************************************************************************************************** */

/* Function MW_read_magick_format() accepts a MagickWand and a potential ImageMagick pseudo-format string and checks
   for the following ImageMagick pseudo-formats:
      VID:
      NULL:
      TILE:
      LABEL:
      MAGICK:
      PLASMA:
      CAPTION:
      FRACTAL:
      PATTERN:
      GRADIENT:

   If one of the pseudo-formats is found, an attempt to read it into the MagickWand is made. If the "VID:" or
   "TILE:" pseudo-formats are found, the rest of the format string is supposed to be a filename, and it is checked
   for PHP accessibility.

   MW_read_magick_format() returns:
      1 - if a pseudo-format is found, and it is successfuly read
	  0 - if a pseudo-format is NOT found
	 -1 - if a pseudo-format is found, but reading it fails, or it is set to read a file which is NOT accessible by PHP.
*/
static char MW_read_magick_format( MagickWand *mgck_wnd, const char *magick_format )
{
	char *colon_p, *magick_filename;
	int colon_idx, is_magick = 0;
	php_stream *stream;
	long img_idx;
	unsigned long num_imgs;

	TSRMLS_FETCH();

#define PRV_VERIFY_MAGICK_FORMAT_FILE( magick_filename, colon_p, stream, is_magick )  \
{  \
	magick_filename = colon_p + 1;  \
	stream = php_stream_open_wrapper( magick_filename, "rb", MW_STD_STREAM_OPTS, (char **) NULL );  \
\
	if ( stream == (php_stream *) NULL ) {  \
		/* PHP cannot open image with the current filename: output fatal error, with reason */  \
		zend_error( MW_E_ERROR, "%s(): PHP cannot read %s; possible php.ini restrictions",  \
								get_active_function_name(TSRMLS_C), magick_filename );  \
		return -1;  \
	}  \
	else {  \
		MW_DEBUG_NOTICE_EX( "The pseudo-format file \"%s\" was opened by PHP successfully", magick_filename );  \
		php_stream_close( stream );  \
		is_magick = 1;  \
	}  \
}

	colon_p = strchr( magick_format, ':' );

	if ( colon_p != (char *) NULL ) {

		colon_idx = colon_p - magick_format;

		if ( colon_idx == 3 ) {
			if ( strncasecmp( magick_format, "VID", (size_t) colon_idx ) == 0 ) {
				PRV_VERIFY_MAGICK_FORMAT_FILE( magick_filename, colon_p, stream, is_magick );
			}
		}
		else {
			if ( colon_idx == 4 ) {
				if ( strncasecmp( magick_format, "NULL", (size_t) colon_idx ) == 0 ) {
					is_magick = 1;
				}
				else {
					if ( strncasecmp( magick_format, "TILE", (size_t) colon_idx ) == 0 ) {
						PRV_VERIFY_MAGICK_FORMAT_FILE( magick_filename, colon_p, stream, is_magick );
					}
				}
			}
			else {
				if ( colon_idx == 5 ) {
					if ( strncasecmp( magick_format, "LABEL", (size_t) colon_idx ) == 0 ) {
						is_magick = 1;
					}
				}
				else {
					if ( colon_idx == 6 ) {
						if (   strncasecmp( magick_format, "MAGICK", (size_t) colon_idx ) == 0
							|| strncasecmp( magick_format, "PLASMA", (size_t) colon_idx ) == 0 )
						{
							is_magick = 1;
						}
					}
					else {
						if ( colon_idx == 7 ) {
							if (   strncasecmp( magick_format, "CAPTION", (size_t) colon_idx ) == 0
								|| strncasecmp( magick_format, "FRACTAL", (size_t) colon_idx ) == 0
								|| strncasecmp( magick_format, "PATTERN", (size_t) colon_idx ) == 0 )
							{
								is_magick = 1;
							}
						}
						else {
							if ( colon_idx == 8 ) {
								if ( strncasecmp( magick_format, "GRADIENT", (size_t) colon_idx ) == 0 ) {
									is_magick = 1;
								}
							}
						}
					}
				}
			}
		}
	}

	if ( is_magick == 1 ) {

		img_idx = (long) MagickGetImageIndex( mgck_wnd );
		if ( mgck_wnd->exception.severity == UndefinedException ) {
			img_idx++;
		}
		num_imgs = (unsigned long) MagickGetNumberImages( mgck_wnd );

		if ( MagickReadImage( mgck_wnd, magick_format ) == MagickTrue ) {
			if ( MagickSetImageIndex( mgck_wnd, (long) img_idx ) == MagickTrue ) {

				num_imgs = ((unsigned long) MagickGetNumberImages( mgck_wnd )) - num_imgs - 1;

				MagickSetImageFilename( mgck_wnd, "" );
				while ( num_imgs > 0 && MagickNextImage( mgck_wnd ) == MagickTrue ) {
					MagickSetImageFilename( mgck_wnd, "" );
					num_imgs--;
				}
			}
			MagickClearException( mgck_wnd );

			MW_DEBUG_NOTICE_EX( "The pseudo-format \"%s\" was read successfully", magick_format );

			return 1;
		}
		else {
			/* C API cannot read the current format: output error, with reason */
			MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
													"cannot read the format \"%s\"",
													magick_format
			);
		}

		return -1;
	}

	return 0;
}

/* ************************************************************************************************************** */

static MagickBooleanType MW_read_image_from_php_stream( MagickWand *mgck_wnd, php_stream *stream, const char *filename_for_err )
{
	FILE *FILE_P;
	long img_idx;
	unsigned long num_imgs;

	TSRMLS_FETCH();

	/* Try and avoid allocating a FILE* if the stream is not naturally a FILE* */
	if ( php_stream_is( stream, PHP_STREAM_IS_STDIO ) ) {

		if (   SUCCESS == php_stream_cast( stream, PHP_STREAM_AS_STDIO, (void **) &FILE_P, REPORT_ERRORS )
			&& FILE_P != (FILE *) NULL )
		{
			img_idx = (long) MagickGetImageIndex( mgck_wnd );
			if ( mgck_wnd->exception.severity == UndefinedException ) {
				img_idx++;
			}
			num_imgs = (unsigned long) MagickGetNumberImages( mgck_wnd );

			if ( MagickReadImageFile( mgck_wnd, FILE_P ) == MagickTrue ) {
				if ( MagickSetImageIndex( mgck_wnd, (long) img_idx ) == MagickTrue ) {

					num_imgs = ((unsigned long) MagickGetNumberImages( mgck_wnd )) - num_imgs - 1;

					MagickSetImageFilename( mgck_wnd, "" );
					while ( num_imgs > 0 && MagickNextImage( mgck_wnd ) == MagickTrue ) {
						MagickSetImageFilename( mgck_wnd, "" );
						num_imgs--;
					}
				}
				MagickClearException( mgck_wnd );
				return MagickTrue;
			}
			else {
				/* C API cannot read image from the current file: output error, with reason */
				MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
														"cannot read the file \"%s\"",
														filename_for_err
				);

				return MagickFalse;
			}
		}
		else {
			zend_error( MW_E_ERROR, "%s(): cannot convert PHP stream to FILE*; cannot read %s",
									get_active_function_name(TSRMLS_C), filename_for_err );
			return MagickFalse;
		}
	}
	else {
		zend_error( MW_E_ERROR, "%s(): PHP stream cannot be cast to a FILE*; was cannot read %s",
								get_active_function_name(TSRMLS_C), filename_for_err );
		return MagickFalse;
	}
}

/* ************************************************************************************************************** */

/* The MagickWand C API actually MagickWriteImageFile()'s the current active image to a filehandle in the original
   image's format, unless the image's filename is empty, so enter the following rigamorole, which saves the image's
   current filename, then sets the image's filename to "", writes the image, and then sets the filename back to the
   saved one.
*/
static MagickBooleanType
		   MW_write_image_to_php_stream( MagickWand *mgck_wnd, php_stream *stream, const long img_idx, const char *filename_for_err )
{
	FILE *FILE_P;
	int img_filename_valid;
	char *orig_img_filename;

	TSRMLS_FETCH();

	if ( php_stream_is( stream, PHP_STREAM_IS_STDIO) )  /* Avoid allocating a FILE* if the stream is not naturally a FILE* */
	{
		if (   php_stream_cast( stream, PHP_STREAM_AS_STDIO, (void **) &FILE_P, REPORT_ERRORS ) == SUCCESS
			&& FILE_P != (FILE *) NULL )
		{
			orig_img_filename = (char *) MagickGetImageFilename( mgck_wnd );
			img_filename_valid = !(orig_img_filename == (char *) NULL || *orig_img_filename == '\0');

			if ( img_filename_valid ) {
				MagickSetImageFilename( mgck_wnd, "" );
			}

			MagickClearException( mgck_wnd );

			if ( MagickWriteImageFile( mgck_wnd, FILE_P ) == MagickFalse )
			{
				/* C API cannot write image to the current filename: output error, with reason */
				MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_2(	mgck_wnd, MagickGetException,
														"cannot write the image at MagickWand index %d to filename \"%s\"",
														img_idx, filename_for_err
				);

				MW_FREE_MAGICK_MEM( char *, orig_img_filename );
				return MagickFalse;
			}

			if ( img_filename_valid ) {
				MagickSetImageFilename( mgck_wnd, orig_img_filename );
			}
			MW_FREE_MAGICK_MEM( char *, orig_img_filename );
			return MagickTrue;
		}
		else {
			zend_error( MW_E_ERROR, "%s(): cannot convert PHP stream to FILE*; cannot write the image at MagickWand index %d to %s",
									get_active_function_name(TSRMLS_C), img_idx, filename_for_err );
			return MagickFalse;
		}
	}
	else {
		zend_error( MW_E_ERROR, "%s(): PHP stream cannot be cast to a FILE*; cannot write the image at MagickWand index %d to %s",
								get_active_function_name(TSRMLS_C), img_idx, filename_for_err );
		return MagickFalse;
	}
}

/* ************************************************************************************************************** */

/* The MagickWand C API actually MagickWriteImagesFile()'s the current active image to a filehandle in the original
   image's format, unless the image's filename is empty, so enter the following rigamorole, which saves the
   MagickWand's current filename, then sets the MagickWand's filename to "", writes the image, and then sets the
   filename back to the saved one.
*/
static MagickBooleanType MW_write_images_to_php_stream( MagickWand *mgck_wnd, php_stream *stream, const char *filename_for_err )
{
	FILE *FILE_P;
	int filename_valid;
	char *orig_filename;

	TSRMLS_FETCH();

	if ( php_stream_is( stream, PHP_STREAM_IS_STDIO) )  /* Avoid allocating a FILE* if the stream is not naturally a FILE* */
	{
		if (   php_stream_cast( stream, PHP_STREAM_AS_STDIO, (void **) &FILE_P, REPORT_ERRORS ) == SUCCESS
			&& FILE_P != (FILE *) NULL )
		{
			orig_filename = (char *) MagickGetFilename( mgck_wnd );
			filename_valid = !(orig_filename == (char *) NULL || *orig_filename == '\0');

			if ( filename_valid ) {
				MagickSetFilename( mgck_wnd, "" );
			}

			MagickClearException( mgck_wnd );

			if ( MagickWriteImagesFile( mgck_wnd, FILE_P ) == MagickFalse )
			{
				/* C API cannot write image to the current filename: output error, with reason */
				MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
														"unable to write the MagickWand's images to filename \"%s\"",
														filename_for_err
				);

				MW_FREE_MAGICK_MEM( char *, orig_filename );
				return MagickFalse;
			}

			if ( filename_valid ) {
				MagickSetFilename( mgck_wnd, orig_filename );
			}

			MW_FREE_MAGICK_MEM( char *, orig_filename );
			return MagickTrue;
		}
		else {
			zend_error( MW_E_ERROR, "%s(): cannot convert PHP stream to FILE*; cannot write the MagickWand's images to %s",
									get_active_function_name(TSRMLS_C), filename_for_err );
			return MagickFalse;
		}
	}
	else {
		zend_error( MW_E_ERROR, "%s(): PHP stream cannot be cast to a FILE*; cannot write the MagickWand's images to %s",
								get_active_function_name(TSRMLS_C), filename_for_err );
		return MagickFalse;
	}
}

/* ************************************************************************************************************** */

/* magickwand_functions[]	=>	Every user visible function must have an entry in magickwand_functions[].
 */
zend_function_entry magickwand_functions[] =
{
	ZEND_FE( wandgetexception, NULL )
	ZEND_FE( wandgetexceptionstring, NULL )
	ZEND_FE( wandgetexceptiontype, NULL )
	ZEND_FE( wandhasexception, NULL )

	ZEND_FE( isdrawingwand, NULL )
	ZEND_FE( ismagickwand, NULL )
	ZEND_FE( ispixeliterator, NULL )
	ZEND_FE( ispixelwand, NULL )


	/* DrawingWand functions */

	ZEND_FE( cleardrawingwand, NULL )
	ZEND_FE( clonedrawingwand, NULL )
	ZEND_FE( destroydrawingwand, NULL )
	ZEND_FE( drawaffine, NULL )
	ZEND_FE( drawannotation, NULL )
	ZEND_FE( drawarc, NULL )

	ZEND_FE( drawbezier, NULL )

	ZEND_FE( drawcircle, NULL )
	ZEND_FE( drawcolor, NULL )
	ZEND_FE( drawcomment, NULL )
	ZEND_FE( drawcomposite, NULL )
	ZEND_FE( drawellipse, NULL )
	ZEND_FE( drawgetclippath, NULL )
	ZEND_FE( drawgetcliprule, NULL )
	ZEND_FE( drawgetclipunits, NULL )

	ZEND_FE( drawgetexception, NULL )
	ZEND_FE( drawgetexceptionstring, NULL )
	ZEND_FE( drawgetexceptiontype, NULL )

	ZEND_FE( drawgetfillalpha, NULL )
	ZEND_FE( drawgetfillopacity, NULL )

	ZEND_FE( drawgetfillcolor, NULL )
	ZEND_FE( drawgetfillrule, NULL )
	ZEND_FE( drawgetfont, NULL )
	ZEND_FE( drawgetfontfamily, NULL )
	ZEND_FE( drawgetfontsize, NULL )
	ZEND_FE( drawgetfontstretch, NULL )
	ZEND_FE( drawgetfontstyle, NULL )
	ZEND_FE( drawgetfontweight, NULL )
	ZEND_FE( drawgetgravity, NULL )

	ZEND_FE( drawgetstrokealpha, NULL )
	ZEND_FE( drawgetstrokeopacity, NULL )

	ZEND_FE( drawgetstrokeantialias, NULL )
	ZEND_FE( drawgetstrokecolor, NULL )
	ZEND_FE( drawgetstrokedasharray, NULL )
	ZEND_FE( drawgetstrokedashoffset, NULL )
	ZEND_FE( drawgetstrokelinecap, NULL )
	ZEND_FE( drawgetstrokelinejoin, NULL )
	ZEND_FE( drawgetstrokemiterlimit, NULL )
	ZEND_FE( drawgetstrokewidth, NULL )
	ZEND_FE( drawgettextalignment, NULL )
	ZEND_FE( drawgettextantialias, NULL )
	ZEND_FE( drawgettextdecoration, NULL )
	ZEND_FE( drawgettextencoding, NULL )

	ZEND_FE( drawgetvectorgraphics, NULL )

	ZEND_FE( drawgettextundercolor, NULL )
	ZEND_FE( drawline, NULL )
	ZEND_FE( drawmatte, NULL )
	ZEND_FE( drawpathclose, NULL )
	ZEND_FE( drawpathcurvetoabsolute, NULL )
	ZEND_FE( drawpathcurvetorelative, NULL )
	ZEND_FE( drawpathcurvetoquadraticbezierabsolute, NULL )
	ZEND_FE( drawpathcurvetoquadraticbezierrelative, NULL )
	ZEND_FE( drawpathcurvetoquadraticbeziersmoothabsolute, NULL )
	ZEND_FE( drawpathcurvetoquadraticbeziersmoothrelative, NULL )
	ZEND_FE( drawpathcurvetosmoothabsolute, NULL )
	ZEND_FE( drawpathcurvetosmoothrelative, NULL )
	ZEND_FE( drawpathellipticarcabsolute, NULL )
	ZEND_FE( drawpathellipticarcrelative, NULL )
	ZEND_FE( drawpathfinish, NULL )
	ZEND_FE( drawpathlinetoabsolute, NULL )
	ZEND_FE( drawpathlinetorelative, NULL )
	ZEND_FE( drawpathlinetohorizontalabsolute, NULL )
	ZEND_FE( drawpathlinetohorizontalrelative, NULL )
	ZEND_FE( drawpathlinetoverticalabsolute, NULL )
	ZEND_FE( drawpathlinetoverticalrelative, NULL )
	ZEND_FE( drawpathmovetoabsolute, NULL )
	ZEND_FE( drawpathmovetorelative, NULL )
	ZEND_FE( drawpathstart, NULL )
	ZEND_FE( drawpoint, NULL )

	ZEND_FE( drawpolygon, NULL )

	ZEND_FE( drawpolyline, NULL )

	ZEND_FE( drawpopclippath, NULL )
	ZEND_FE( drawpopdefs, NULL )
	ZEND_FE( drawpoppattern, NULL )
	ZEND_FE( drawpushclippath, NULL )
	ZEND_FE( drawpushdefs, NULL )
	ZEND_FE( drawpushpattern, NULL )
	ZEND_FE( drawrectangle, NULL )
	ZEND_FE( drawrender, NULL )
	ZEND_FE( drawrotate, NULL )
	ZEND_FE( drawroundrectangle, NULL )
	ZEND_FE( drawscale, NULL )
	ZEND_FE( drawsetclippath, NULL )
	ZEND_FE( drawsetcliprule, NULL )
	ZEND_FE( drawsetclipunits, NULL )

	ZEND_FE( drawsetfillalpha, NULL )
	ZEND_FE( drawsetfillopacity, NULL )

	ZEND_FE( drawsetfillcolor, NULL )
	ZEND_FE( drawsetfillpatternurl, NULL )
	ZEND_FE( drawsetfillrule, NULL )
	ZEND_FE( drawsetfont, NULL )
	ZEND_FE( drawsetfontfamily, NULL )
	ZEND_FE( drawsetfontsize, NULL )
	ZEND_FE( drawsetfontstretch, NULL )
	ZEND_FE( drawsetfontstyle, NULL )
	ZEND_FE( drawsetfontweight, NULL )
	ZEND_FE( drawsetgravity, NULL )

	ZEND_FE( drawsetstrokealpha, NULL )
	ZEND_FE( drawsetstrokeopacity, NULL )

	ZEND_FE( drawsetstrokeantialias, NULL )
	ZEND_FE( drawsetstrokecolor, NULL )

	ZEND_FE( drawsetstrokedasharray, NULL )

	ZEND_FE( drawsetstrokedashoffset, NULL )
	ZEND_FE( drawsetstrokelinecap, NULL )
	ZEND_FE( drawsetstrokelinejoin, NULL )
	ZEND_FE( drawsetstrokemiterlimit, NULL )
	ZEND_FE( drawsetstrokepatternurl, NULL )
	ZEND_FE( drawsetstrokewidth, NULL )
	ZEND_FE( drawsettextalignment, NULL )
	ZEND_FE( drawsettextantialias, NULL )
	ZEND_FE( drawsettextdecoration, NULL )
	ZEND_FE( drawsettextencoding, NULL )
	ZEND_FE( drawsettextundercolor, NULL )
	ZEND_FE( drawsetvectorgraphics, NULL )
	ZEND_FE( drawskewx, NULL )
	ZEND_FE( drawskewy, NULL )
	ZEND_FE( drawtranslate, NULL )
	ZEND_FE( drawsetviewbox, NULL )
	ZEND_FE( newdrawingwand, NULL )
	ZEND_FE( popdrawingwand, NULL )
	ZEND_FE( pushdrawingwand, NULL )


	/* MagickWand functions */

	ZEND_FE( clearmagickwand, NULL )
	ZEND_FE( clonemagickwand, NULL )
	ZEND_FE( destroymagickwand, NULL )
	ZEND_FE( magickadaptivethresholdimage, NULL )

/* OW modification from C API convention:
      PHP's magickaddimage adds only the current active image from one wand to another,
	  while PHP's magickaddimages replicates the functionality of the C API's
	  MagickAddImage(), adding all the images of one wnad to another.

	  Minor semantics, but original funtionality proved annoying in actual usage.
*/
	ZEND_FE( magickaddimage, NULL )
	ZEND_FE( magickaddimages, NULL )

	ZEND_FE( magickaddnoiseimage, NULL )
	ZEND_FE( magickaffinetransformimage, NULL )
	ZEND_FE( magickannotateimage, NULL )
	ZEND_FE( magickappendimages, NULL )
	ZEND_FE( magickaverageimages, NULL )
	ZEND_FE( magickblackthresholdimage, NULL )

	ZEND_FE( magickblurimage, NULL )
/*	ZEND_FE( magickblurimagechannel, NULL )  */

	ZEND_FE( magickborderimage, NULL )
	ZEND_FE( magickcharcoalimage, NULL )
	ZEND_FE( magickchopimage, NULL )
	ZEND_FE( magickclipimage, NULL )
	ZEND_FE( magickclippathimage, NULL )
	ZEND_FE( magickcoalesceimages, NULL )
	ZEND_FE( magickcolorfloodfillimage, NULL )
	ZEND_FE( magickcolorizeimage, NULL )
	ZEND_FE( magickcombineimages, NULL )
	ZEND_FE( magickcommentimage, NULL )

	ZEND_FE( magickcompareimages, NULL )
/*	ZEND_FE( magickcompareimagechannels, NULL )  */

	ZEND_FE( magickcompositeimage, NULL )
	ZEND_FE( magickconstituteimage, NULL )
	ZEND_FE( magickcontrastimage, NULL )

	ZEND_FE( magickconvolveimage, NULL )
/*	ZEND_FE( magickconvolveimagechannel, NULL )  */

	ZEND_FE( magickcropimage, NULL )
	ZEND_FE( magickcyclecolormapimage, NULL )
	ZEND_FE( magickdeconstructimages, NULL )
	ZEND_FE( magickdescribeimage, NULL )
	ZEND_FE( magickdespeckleimage, NULL )
	ZEND_FE( magickdrawimage, NULL )

	ZEND_FE( magickechoimageblob, NULL )
	ZEND_FE( magickechoimagesblob, NULL )

	ZEND_FE( magickedgeimage, NULL )
	ZEND_FE( magickembossimage, NULL )
	ZEND_FE( magickenhanceimage, NULL )
	ZEND_FE( magickequalizeimage, NULL )

	ZEND_FE( magickevaluateimage, NULL )
/*	ZEND_FE( magickevaluateimagechannel, NULL )  */

	ZEND_FE( magickflattenimages, NULL )
	ZEND_FE( magickflipimage, NULL )
	ZEND_FE( magickflopimage, NULL )
	ZEND_FE( magickframeimage, NULL )

	ZEND_FE( magickfximage, NULL )
/*	ZEND_FE( magickfximagechannel, NULL )  */

	ZEND_FE( magickgammaimage, NULL )
/*	ZEND_FE( magickgammaimagechannel, NULL )  */

	ZEND_FE( magickgaussianblurimage, NULL )
/*	ZEND_FE( magickgaussianblurimagechannel, NULL )  */

	ZEND_FE( magickgetcopyright, NULL )

	ZEND_FE( magickgetexception, NULL )
	ZEND_FE( magickgetexceptionstring, NULL )
	ZEND_FE( magickgetexceptiontype, NULL )

	ZEND_FE( magickgetfilename, NULL )
	ZEND_FE( magickgetformat, NULL )
	ZEND_FE( magickgethomeurl, NULL )
	ZEND_FE( magickgetimage, NULL )
	ZEND_FE( magickgetimagebackgroundcolor, NULL )
	ZEND_FE( magickgetimageblob, NULL )
	ZEND_FE( magickgetimagesblob, NULL )
	ZEND_FE( magickgetimageblueprimary, NULL )
	ZEND_FE( magickgetimagebordercolor, NULL )

	ZEND_FE( magickgetimagechannelmean, NULL )
	ZEND_FE( magickgetimagecolormapcolor, NULL )
	ZEND_FE( magickgetimagecolors, NULL )
	ZEND_FE( magickgetimagecolorspace, NULL )
	ZEND_FE( magickgetimagecompose, NULL )
	ZEND_FE( magickgetimagecompression, NULL )
	ZEND_FE( magickgetimagecompressionquality, NULL )
	ZEND_FE( magickgetimagedelay, NULL )

	ZEND_FE( magickgetimagedepth, NULL )
/*	ZEND_FE( magickgetimagechanneldepth, NULL )  */

	ZEND_FE( magickgetimagedistortion, NULL )
/*	ZEND_FE( magickgetimagechanneldistortion, NULL )	*/

	ZEND_FE( magickgetimagedispose, NULL )

	ZEND_FE( magickgetimageextrema, NULL )
/*	ZEND_FE( magickgetimagechannelextrema, NULL )  */

	ZEND_FE( magickgetimagefilename, NULL )
	ZEND_FE( magickgetimageformat, NULL )
	ZEND_FE( magickgetimagegamma, NULL )
	ZEND_FE( magickgetimagegreenprimary, NULL )
	ZEND_FE( magickgetimageheight, NULL )
	ZEND_FE( magickgetimagehistogram, NULL )
	ZEND_FE( magickgetimageindex, NULL )
	ZEND_FE( magickgetimageinterlacescheme, NULL )
	ZEND_FE( magickgetimageiterations, NULL )
	ZEND_FE( magickgetimagemattecolor, NULL )

	ZEND_FE( magickgetimagemimetype, NULL )
	ZEND_FE( magickgetmimetype, NULL )

	ZEND_FE( magickgetimagepixels, NULL )
	ZEND_FE( magickgetimageprofile, NULL )
	ZEND_FE( magickgetimageredprimary, NULL )
	ZEND_FE( magickgetimagerenderingintent, NULL )
	ZEND_FE( magickgetimageresolution, NULL )
	ZEND_FE( magickgetimagescene, NULL )
	ZEND_FE( magickgetimagesignature, NULL )
	ZEND_FE( magickgetimagesize, NULL )
	ZEND_FE( magickgetimagetype, NULL )
	ZEND_FE( magickgetimageunits, NULL )
	ZEND_FE( magickgetimagevirtualpixelmethod, NULL )
	ZEND_FE( magickgetimagewhitepoint, NULL )
	ZEND_FE( magickgetimagewidth, NULL )
	ZEND_FE( magickgetinterlacescheme, NULL )
	ZEND_FE( magickgetnumberimages, NULL )
	ZEND_FE( magickgetpackagename, NULL )
	ZEND_FE( magickgetquantumdepth, NULL )
	ZEND_FE( magickgetreleasedate, NULL )
	ZEND_FE( magickgetresourcelimit, NULL )
	ZEND_FE( magickgetsamplingfactors, NULL )

/*	ZEND_FE( magickgetsize, NULL ) */
	ZEND_FALIAS( magickgetsize, magickgetwandsize, NULL )
	ZEND_FE( magickgetwandsize, NULL )

	ZEND_FE( magickgetversion, NULL )

/* OW added for convenience in getting the version as a number or string */
	ZEND_FE( magickgetversionnumber, NULL )
	ZEND_FE( magickgetversionstring, NULL )
/* end convenience functions */

	ZEND_FE( magickhasnextimage, NULL )
	ZEND_FE( magickhaspreviousimage, NULL )
	ZEND_FE( magickimplodeimage, NULL )
	ZEND_FE( magicklabelimage, NULL )

	ZEND_FE( magicklevelimage, NULL )
/*	ZEND_FE( magicklevelimagechannel, NULL )  */

	ZEND_FE( magickmagnifyimage, NULL )
	ZEND_FE( magickmapimage, NULL )
	ZEND_FE( magickmattefloodfillimage, NULL )
	ZEND_FE( magickmedianfilterimage, NULL )
	ZEND_FE( magickminifyimage, NULL )
	ZEND_FE( magickmodulateimage, NULL )
	ZEND_FE( magickmontageimage, NULL )
	ZEND_FE( magickmorphimages, NULL )
	ZEND_FE( magickmosaicimages, NULL )
	ZEND_FE( magickmotionblurimage, NULL )

	ZEND_FE( magicknegateimage, NULL )
/*	ZEND_FE( magicknegateimagechannel, NULL )  */

	ZEND_FE( magicknewimage, NULL )

	ZEND_FE( magicknextimage, NULL )
	ZEND_FE( magicknormalizeimage, NULL )
	ZEND_FE( magickoilpaintimage, NULL )

	ZEND_FE( magickpaintopaqueimage, NULL )
	ZEND_FE( magickpainttransparentimage, NULL )

	ZEND_FE( magickpingimage, NULL )
	ZEND_FE( magickposterizeimage, NULL )
	ZEND_FE( magickpreviewimages, NULL )
	ZEND_FE( magickpreviousimage, NULL )
	ZEND_FE( magickprofileimage, NULL )
	ZEND_FE( magickquantizeimage, NULL )
	ZEND_FE( magickquantizeimages, NULL )
	ZEND_FE( magickqueryconfigureoption, NULL )
	ZEND_FE( magickqueryconfigureoptions, NULL )
	ZEND_FE( magickqueryfontmetrics, NULL )

/* the following functions are convenience functions I implemented, as alternatives to calling MagickQueryFontMetrics */
	ZEND_FE( magickgetcharwidth, NULL )
	ZEND_FE( magickgetcharheight, NULL )
	ZEND_FE( magickgettextascent, NULL )
	ZEND_FE( magickgettextdescent, NULL )
	ZEND_FE( magickgetstringwidth, NULL )
	ZEND_FE( magickgetstringheight, NULL )
	ZEND_FE( magickgetmaxtextadvance, NULL )
/* end of convenience functions */

	ZEND_FE( magickqueryfonts, NULL )
	ZEND_FE( magickqueryformats, NULL )
	ZEND_FE( magickradialblurimage, NULL )
	ZEND_FE( magickraiseimage, NULL )
	ZEND_FE( magickreadimage, NULL )
	ZEND_FE( magickreadimageblob, NULL )
	ZEND_FE( magickreadimagefile, NULL )

	/* Custom PHP function; accepts a PHP array of filenames and attempts to read them all into the MagickWand */
	ZEND_FE( magickreadimages, NULL )

	ZEND_FE( magickreducenoiseimage, NULL )
	ZEND_FE( magickremoveimage, NULL )
	ZEND_FE( magickremoveimageprofile, NULL )

	ZEND_FE( magickremoveimageprofiles, NULL )

	ZEND_FE( magickresetiterator, NULL )
	ZEND_FE( magickresampleimage, NULL )
	ZEND_FE( magickresizeimage, NULL )
	ZEND_FE( magickrollimage, NULL )
	ZEND_FE( magickrotateimage, NULL )
	ZEND_FE( magicksampleimage, NULL )
	ZEND_FE( magickscaleimage, NULL )
	ZEND_FE( magickseparateimagechannel, NULL )
	ZEND_FE( magicksetcompressionquality, NULL )
	ZEND_FE( magicksetfilename, NULL )
	ZEND_FE( magicksetfirstiterator, NULL )
	ZEND_FE( magicksetformat, NULL )

	ZEND_FE( magicksetimage, NULL )
	ZEND_FE( magicksetimagebackgroundcolor, NULL )
	ZEND_FE( magicksetimagebias, NULL )
	ZEND_FE( magicksetimageblueprimary, NULL )
	ZEND_FE( magicksetimagebordercolor, NULL )
	ZEND_FE( magicksetimagecolormapcolor, NULL )
	ZEND_FE( magicksetimagecolorspace, NULL )
	ZEND_FE( magicksetimagecompose, NULL )
	ZEND_FE( magicksetimagecompression, NULL )
	ZEND_FE( magicksetimagecompressionquality, NULL )
	ZEND_FE( magicksetimagedelay, NULL )

	ZEND_FE( magicksetimagedepth, NULL )
/*	ZEND_FE( magicksetimagechanneldepth, NULL )  */

	ZEND_FE( magicksetimagedispose, NULL )
	ZEND_FE( magicksetimagefilename, NULL )

	ZEND_FE( magicksetimageformat, NULL )

	ZEND_FE( magicksetimagegamma, NULL )
	ZEND_FE( magicksetimagegreenprimary, NULL )
	ZEND_FE( magicksetimageindex, NULL )
	ZEND_FE( magicksetimageinterlacescheme, NULL )
	ZEND_FE( magicksetimageiterations, NULL )
	ZEND_FE( magicksetimagemattecolor, NULL )
	ZEND_FE( magicksetimageoption, NULL )
	ZEND_FE( magicksetimagepixels, NULL )
	ZEND_FE( magicksetimageprofile, NULL )
	ZEND_FE( magicksetimageredprimary, NULL )
	ZEND_FE( magicksetimagerenderingintent, NULL )
	ZEND_FE( magicksetimageresolution, NULL )
	ZEND_FE( magicksetimagescene, NULL )
	ZEND_FE( magicksetimagetype, NULL )
	ZEND_FE( magicksetimageunits, NULL )
	ZEND_FE( magicksetimagevirtualpixelmethod, NULL )
	ZEND_FE( magicksetimagewhitepoint, NULL )

	ZEND_FE( magicksetinterlacescheme, NULL )
	ZEND_FE( magicksetlastiterator, NULL )
	ZEND_FE( magicksetpassphrase, NULL )
	ZEND_FE( magicksetresolution, NULL )
	ZEND_FE( magicksetresourcelimit, NULL )
	ZEND_FE( magicksetsamplingfactors, NULL )

/*	ZEND_FE( magicksetsize, NULL ) */
	ZEND_FALIAS( magicksetsize, magicksetwandsize, NULL )

	ZEND_FE( magicksetwandsize, NULL )

	ZEND_FE( magickshadowimage, NULL )

	ZEND_FE( magicksharpenimage, NULL )
/*	ZEND_FE( magicksharpenimagechannel, NULL ) */

	ZEND_FE( magickshaveimage, NULL )
	ZEND_FE( magickshearimage, NULL )
	ZEND_FE( magicksolarizeimage, NULL )
	ZEND_FE( magickspliceimage, NULL )
	ZEND_FE( magickspreadimage, NULL )
	ZEND_FE( magicksteganoimage, NULL )
	ZEND_FE( magickstereoimage, NULL )
	ZEND_FE( magickstripimage, NULL )
	ZEND_FE( magickswirlimage, NULL )
	ZEND_FE( magicktextureimage, NULL )

	ZEND_FE( magickthresholdimage, NULL )
/*	ZEND_FE( magickthresholdimagechannel, NULL ) */

	ZEND_FE( magicktintimage, NULL )
	ZEND_FE( magicktransformimage, NULL )
	ZEND_FE( magicktrimimage, NULL )

	ZEND_FE( magickunsharpmaskimage, NULL )
/*	ZEND_FE( magickunsharpmaskimagechannel, NULL ) */

	ZEND_FE( magickwaveimage, NULL )
	ZEND_FE( magickwhitethresholdimage, NULL )
	ZEND_FE( magickwriteimage, NULL )
	ZEND_FE( magickwriteimagefile, NULL )
	ZEND_FE( magickwriteimages, NULL )
	ZEND_FE( newmagickwand, NULL )

		
	/* PixelIterator functions */

	ZEND_FE( clearpixeliterator, NULL )
	ZEND_FE( destroypixeliterator, NULL )
	ZEND_FE( newpixeliterator, NULL )
	ZEND_FE( newpixelregioniterator, NULL )

	ZEND_FE( pixelgetiteratorexception, NULL )
	ZEND_FE( pixelgetiteratorexceptionstring, NULL )
	ZEND_FE( pixelgetiteratorexceptiontype, NULL )

	ZEND_FE( pixelgetnextiteratorrow, NULL )
	ZEND_FE( pixelresetiterator, NULL )
	ZEND_FE( pixelsetiteratorrow, NULL )
	ZEND_FE( pixelsynciterator, NULL )


	/* PixelWand functions */

	ZEND_FE( clearpixelwand, NULL )
	ZEND_FE( destroypixelwand, NULL )

/************************** OW_Modified *************************
MagickWand's destroypixelwandarray() == ImageMagick's destroypixelwands
*/
/*	ZEND_FE( destroypixelwands, NULL ) */
	ZEND_FALIAS( destroypixelwands, destroypixelwandarray, NULL )

	ZEND_FE( destroypixelwandarray, NULL )

	ZEND_FE( newpixelwand, NULL )

/************************** OW_Modified *************************
MagickWand's newpixelwandarray() == ImageMagick's newpixelwands
*/
	ZEND_FALIAS( newpixelwands, newpixelwandarray, NULL )
	ZEND_FE( newpixelwandarray, NULL )

	ZEND_FE( pixelgetalpha, NULL )
	ZEND_FE( pixelgetalphaquantum, NULL )

	ZEND_FE( pixelgetexception, NULL )
	ZEND_FE( pixelgetexceptionstring, NULL )
	ZEND_FE( pixelgetexceptiontype, NULL )

	ZEND_FE( pixelgetblack, NULL )
	ZEND_FE( pixelgetblackquantum, NULL )
	ZEND_FE( pixelgetblue, NULL )
	ZEND_FE( pixelgetbluequantum, NULL )
	ZEND_FE( pixelgetcolorasstring, NULL )
	ZEND_FE( pixelgetcolorcount, NULL )
	ZEND_FE( pixelgetcyan, NULL )
	ZEND_FE( pixelgetcyanquantum, NULL )
	ZEND_FE( pixelgetgreen, NULL )
	ZEND_FE( pixelgetgreenquantum, NULL )
	ZEND_FE( pixelgetindex, NULL )
	ZEND_FE( pixelgetmagenta, NULL )
	ZEND_FE( pixelgetmagentaquantum, NULL )
	ZEND_FE( pixelgetopacity, NULL )
	ZEND_FE( pixelgetopacityquantum, NULL )
	ZEND_FE( pixelgetquantumcolor, NULL )
	ZEND_FE( pixelgetred, NULL )
	ZEND_FE( pixelgetredquantum, NULL )
	ZEND_FE( pixelgetyellow, NULL )
	ZEND_FE( pixelgetyellowquantum, NULL )
	ZEND_FE( pixelsetalpha, NULL )
	ZEND_FE( pixelsetalphaquantum, NULL )
	ZEND_FE( pixelsetblack, NULL )
	ZEND_FE( pixelsetblackquantum, NULL )
	ZEND_FE( pixelsetblue, NULL )
	ZEND_FE( pixelsetbluequantum, NULL )
	ZEND_FE( pixelsetcolor, NULL )
	ZEND_FE( pixelsetcolorcount, NULL )
	ZEND_FE( pixelsetcyan, NULL )
	ZEND_FE( pixelsetcyanquantum, NULL )
	ZEND_FE( pixelsetgreen, NULL )
	ZEND_FE( pixelsetgreenquantum, NULL )
	ZEND_FE( pixelsetindex, NULL )
	ZEND_FE( pixelsetmagenta, NULL )
	ZEND_FE( pixelsetmagentaquantum, NULL )
	ZEND_FE( pixelsetopacity, NULL )
	ZEND_FE( pixelsetopacityquantum, NULL )
	ZEND_FE( pixelsetquantumcolor, NULL )
	ZEND_FE( pixelsetred, NULL )
	ZEND_FE( pixelsetredquantum, NULL )
	ZEND_FE( pixelsetyellow, NULL )
	ZEND_FE( pixelsetyellowquantum, NULL )

	{NULL, NULL, NULL}	/* Must be the last line in magickwand_functions[] */
};

/* magickwand_module_entry
 */
zend_module_entry magickwand_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"magickwand",
	magickwand_functions,
	ZEND_MINIT( magickwand ),
	NULL,	/* ZEND_MSHUTDOWN( magickwand ),	*/
	NULL,	/* ZEND_RINIT( magickwand ),		*/	/* Replace with NULL if there's nothing to do at request start */
	NULL,	/* ZEND_RSHUTDOWN( magickwand ),	*/	/* Replace with NULL if there's nothing to do at request end */
	ZEND_MINFO( magickwand ),
#if ZEND_MODULE_API_NO >= 20010901
	MAGICKWAND_VERSION, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MAGICKWAND
	ZEND_GET_MODULE( magickwand )
#endif

/* {{{ DrawingWand_destruction_handler
*/
static void DrawingWand_destruction_handler( zend_rsrc_list_entry *rsrc TSRMLS_DC )
{
	MW_DEBUG_NOTICE( "Entered DrawingWand destruction handler" );
	MW_DESTROY_RSRC( DrawingWand, (rsrc->ptr) );
}
/* }}} */

/* {{{ MagickWand_destruction_handler
*/
static void MagickWand_destruction_handler( zend_rsrc_list_entry *rsrc TSRMLS_DC )
{
	MW_DEBUG_NOTICE( "Entered MagickWand destruction handler" );
	MW_DESTROY_RSRC( MagickWand, (rsrc->ptr) );
}
/* }}} */

/* {{{ PixelIterator_destruction_handler
*/
static void PixelIterator_destruction_handler( zend_rsrc_list_entry *rsrc TSRMLS_DC )
{
	MW_DEBUG_NOTICE( "Entered PixelIterator destruction handler" );
	MW_DESTROY_RSRC( PixelIterator, (rsrc->ptr) );
}
/* }}} */

/* {{{ PixelWand_destruction_handler
*/
static void PixelWand_destruction_handler( zend_rsrc_list_entry *rsrc TSRMLS_DC )
{
	MW_DEBUG_NOTICE( "Entered PixelWand destruction handler" );
	MW_DESTROY_RSRC( PixelWand, (rsrc->ptr) );
}
/* }}} */

/* {{{ PixelIteratorPixelWand_destruction_handler
*/
static void PixelIteratorPixelWand_destruction_handler( zend_rsrc_list_entry *rsrc TSRMLS_DC )
{
/*  NOTE: the reason this method does nothing and returns, is due to the fact that the resource
	this function is supposed to destroy is a PixelWand that was included as part of an array
	of PixelWands that were returned by PixelGetNextIteratorRow( pixel_iterator ). The
	particular PixelWand that this function is supposed to destroy, (via the same method as
	PixelWand_destruction_handler() above), is actually DIRECTLY LINKED to the PixelIterator
	the array it was in came from.
	(This, I realized, is because a PixelIterator's PixelWands are ONLY created once, and the
	array they are in is a set size, and when you request different rows of an image, you
	actually get the exact same PixelWands, just with different colors, repesenting the colors
	of the requested row of the image.)
	As a result, destroying it now, would cause ImageMagick to try to destroy it twice, once
	now, and once when the PixelIterator_destruction_handler() above is called to destroy the
	PixelIterator it actually belongs to.
	This causes ImageMagick to cause a KING KAH-MEHA-MEHA SIZED ERROR, (since it will not
	Destroy what does not exist, i.e. what it has already destroyed) which aborts the entire
	program, (via an assert() call), in the middle of PHP script execution.

	So, enter the PixelIteratorPixelWand "type", and it's associated handler, which does
	absolutely nothing!
*/
	MW_DEBUG_NOTICE( "Entered PixelIteratorPixelWand destruction handler (doing nothing...)" );
	return;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
*/
ZEND_MINIT_FUNCTION( magickwand )
{
	MW_DEFINE_PHP_RSRC( DrawingWand   );
	MW_DEFINE_PHP_RSRC( MagickWand    );
	MW_DEFINE_PHP_RSRC( PixelIterator );
	MW_DEFINE_PHP_RSRC( PixelWand     );

	MW_DEFINE_PHP_RSRC( PixelIteratorPixelWand );

	/* ImageMagick integer Quantum color constants, registered as double constants in PHP,
	   since they are based on the Quantum datatype, which could be the size of an unsigned long,
	   which PHP does not support
	*/
	MW_REGISTER_DOUBLE_CONSTANT( MaxRGB );
	MW_REGISTER_DOUBLE_CONSTANT( OpaqueOpacity );
	MW_REGISTER_DOUBLE_CONSTANT( TransparentOpacity );

	/***** AlignType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedAlign );
	MW_REGISTER_LONG_CONSTANT( LeftAlign );
	MW_REGISTER_LONG_CONSTANT( CenterAlign );
	MW_REGISTER_LONG_CONSTANT( RightAlign );

	/***** ChannelType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedChannel );
	MW_REGISTER_LONG_CONSTANT( RedChannel );
	MW_REGISTER_LONG_CONSTANT( CyanChannel );
	MW_REGISTER_LONG_CONSTANT( GreenChannel );
	MW_REGISTER_LONG_CONSTANT( MagentaChannel );
	MW_REGISTER_LONG_CONSTANT( BlueChannel );
	MW_REGISTER_LONG_CONSTANT( YellowChannel );
	MW_REGISTER_LONG_CONSTANT( AlphaChannel );
	MW_REGISTER_LONG_CONSTANT( OpacityChannel );
	MW_REGISTER_LONG_CONSTANT( BlackChannel );
	MW_REGISTER_LONG_CONSTANT( IndexChannel );
	MW_REGISTER_LONG_CONSTANT( AllChannels );

	/***** ClipPathUnits *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedPathUnits );
	MW_REGISTER_LONG_CONSTANT( UserSpace );
	MW_REGISTER_LONG_CONSTANT( UserSpaceOnUse );
	MW_REGISTER_LONG_CONSTANT( ObjectBoundingBox );

	/***** ColorspaceType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedColorspace );
	MW_REGISTER_LONG_CONSTANT( RGBColorspace );
	MW_REGISTER_LONG_CONSTANT( GRAYColorspace );
	MW_REGISTER_LONG_CONSTANT( TransparentColorspace );
	MW_REGISTER_LONG_CONSTANT( OHTAColorspace );
	MW_REGISTER_LONG_CONSTANT( LABColorspace );
	MW_REGISTER_LONG_CONSTANT( XYZColorspace );
	MW_REGISTER_LONG_CONSTANT( YCbCrColorspace );
	MW_REGISTER_LONG_CONSTANT( YCCColorspace );
	MW_REGISTER_LONG_CONSTANT( YIQColorspace );
	MW_REGISTER_LONG_CONSTANT( YPbPrColorspace );
	MW_REGISTER_LONG_CONSTANT( YUVColorspace );
	MW_REGISTER_LONG_CONSTANT( CMYKColorspace );
	MW_REGISTER_LONG_CONSTANT( sRGBColorspace );
	MW_REGISTER_LONG_CONSTANT( HSBColorspace );
	MW_REGISTER_LONG_CONSTANT( HSLColorspace );
	MW_REGISTER_LONG_CONSTANT( HWBColorspace );

	/***** CompositeOperator *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedCompositeOp );
	MW_REGISTER_LONG_CONSTANT( NoCompositeOp );
	MW_REGISTER_LONG_CONSTANT( AddCompositeOp );
	MW_REGISTER_LONG_CONSTANT( AtopCompositeOp );
	MW_REGISTER_LONG_CONSTANT( BlendCompositeOp );
	MW_REGISTER_LONG_CONSTANT( BumpmapCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ClearCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ColorBurnCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ColorDodgeCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ColorizeCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyBlackCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyBlueCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyCyanCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyGreenCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyMagentaCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyOpacityCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyRedCompositeOp );
	MW_REGISTER_LONG_CONSTANT( CopyYellowCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DarkenCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DstAtopCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DstCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DstInCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DstOutCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DstOverCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DifferenceCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DisplaceCompositeOp );
	MW_REGISTER_LONG_CONSTANT( DissolveCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ExclusionCompositeOp );
	MW_REGISTER_LONG_CONSTANT( HardLightCompositeOp );
	MW_REGISTER_LONG_CONSTANT( HueCompositeOp );
	MW_REGISTER_LONG_CONSTANT( InCompositeOp );
	MW_REGISTER_LONG_CONSTANT( LightenCompositeOp );
	MW_REGISTER_LONG_CONSTANT( LuminizeCompositeOp );
	MW_REGISTER_LONG_CONSTANT( MinusCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ModulateCompositeOp );
	MW_REGISTER_LONG_CONSTANT( MultiplyCompositeOp );
	MW_REGISTER_LONG_CONSTANT( OutCompositeOp );
	MW_REGISTER_LONG_CONSTANT( OverCompositeOp );
	MW_REGISTER_LONG_CONSTANT( OverlayCompositeOp );
	MW_REGISTER_LONG_CONSTANT( PlusCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ReplaceCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SaturateCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ScreenCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SoftLightCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SrcAtopCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SrcCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SrcInCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SrcOutCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SrcOverCompositeOp );
	MW_REGISTER_LONG_CONSTANT( SubtractCompositeOp );
	MW_REGISTER_LONG_CONSTANT( ThresholdCompositeOp );
	MW_REGISTER_LONG_CONSTANT( XorCompositeOp );

	/***** CompressionType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedCompression );
	MW_REGISTER_LONG_CONSTANT( NoCompression );
	MW_REGISTER_LONG_CONSTANT( BZipCompression );
	MW_REGISTER_LONG_CONSTANT( FaxCompression );
	MW_REGISTER_LONG_CONSTANT( Group4Compression );
	MW_REGISTER_LONG_CONSTANT( JPEGCompression );
	MW_REGISTER_LONG_CONSTANT( LosslessJPEGCompression );
	MW_REGISTER_LONG_CONSTANT( LZWCompression );
	MW_REGISTER_LONG_CONSTANT( RLECompression );
	MW_REGISTER_LONG_CONSTANT( ZipCompression );

	/***** DecorationType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedDecoration );
	MW_REGISTER_LONG_CONSTANT( NoDecoration );
	MW_REGISTER_LONG_CONSTANT( UnderlineDecoration );
	MW_REGISTER_LONG_CONSTANT( OverlineDecoration );
	MW_REGISTER_LONG_CONSTANT( LineThroughDecoration );

	/***** DisposeType *****/
	MW_REGISTER_LONG_CONSTANT( UnrecognizedDispose );
	MW_REGISTER_LONG_CONSTANT( UndefinedDispose );
	MW_REGISTER_LONG_CONSTANT( NoneDispose );
	MW_REGISTER_LONG_CONSTANT( BackgroundDispose );
	MW_REGISTER_LONG_CONSTANT( PreviousDispose );

	/***** ExceptionType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedException );
	MW_REGISTER_LONG_CONSTANT( WarningException );
	MW_REGISTER_LONG_CONSTANT( ResourceLimitWarning );
	MW_REGISTER_LONG_CONSTANT( TypeWarning );
	MW_REGISTER_LONG_CONSTANT( OptionWarning );
	MW_REGISTER_LONG_CONSTANT( DelegateWarning );
	MW_REGISTER_LONG_CONSTANT( MissingDelegateWarning );
	MW_REGISTER_LONG_CONSTANT( CorruptImageWarning );
	MW_REGISTER_LONG_CONSTANT( FileOpenWarning );
	MW_REGISTER_LONG_CONSTANT( BlobWarning );
	MW_REGISTER_LONG_CONSTANT( StreamWarning );
	MW_REGISTER_LONG_CONSTANT( CacheWarning );
	MW_REGISTER_LONG_CONSTANT( CoderWarning );
	MW_REGISTER_LONG_CONSTANT( ModuleWarning );
	MW_REGISTER_LONG_CONSTANT( DrawWarning );
	MW_REGISTER_LONG_CONSTANT( ImageWarning );
	MW_REGISTER_LONG_CONSTANT( WandWarning );
/*	MW_REGISTER_LONG_CONSTANT( XServerWarning ); */
	MW_REGISTER_LONG_CONSTANT( MonitorWarning );
	MW_REGISTER_LONG_CONSTANT( RegistryWarning );
	MW_REGISTER_LONG_CONSTANT( ConfigureWarning );
	MW_REGISTER_LONG_CONSTANT( ErrorException );
	MW_REGISTER_LONG_CONSTANT( ResourceLimitError );
	MW_REGISTER_LONG_CONSTANT( TypeError );
	MW_REGISTER_LONG_CONSTANT( OptionError );
	MW_REGISTER_LONG_CONSTANT( DelegateError );
	MW_REGISTER_LONG_CONSTANT( MissingDelegateError );
	MW_REGISTER_LONG_CONSTANT( CorruptImageError );
	MW_REGISTER_LONG_CONSTANT( FileOpenError );
	MW_REGISTER_LONG_CONSTANT( BlobError );
	MW_REGISTER_LONG_CONSTANT( StreamError );
	MW_REGISTER_LONG_CONSTANT( CacheError );
	MW_REGISTER_LONG_CONSTANT( CoderError );
	MW_REGISTER_LONG_CONSTANT( ModuleError );
	MW_REGISTER_LONG_CONSTANT( DrawError );
	MW_REGISTER_LONG_CONSTANT( ImageError );
	MW_REGISTER_LONG_CONSTANT( WandError );
/*	MW_REGISTER_LONG_CONSTANT( XServerError ); */
	MW_REGISTER_LONG_CONSTANT( MonitorError );
	MW_REGISTER_LONG_CONSTANT( RegistryError );
	MW_REGISTER_LONG_CONSTANT( ConfigureError );
	MW_REGISTER_LONG_CONSTANT( FatalErrorException );
	MW_REGISTER_LONG_CONSTANT( ResourceLimitFatalError );
	MW_REGISTER_LONG_CONSTANT( TypeFatalError );
	MW_REGISTER_LONG_CONSTANT( OptionFatalError );
	MW_REGISTER_LONG_CONSTANT( DelegateFatalError );
	MW_REGISTER_LONG_CONSTANT( MissingDelegateFatalError );
	MW_REGISTER_LONG_CONSTANT( CorruptImageFatalError );
	MW_REGISTER_LONG_CONSTANT( FileOpenFatalError );
	MW_REGISTER_LONG_CONSTANT( BlobFatalError );
	MW_REGISTER_LONG_CONSTANT( StreamFatalError );
	MW_REGISTER_LONG_CONSTANT( CacheFatalError );
	MW_REGISTER_LONG_CONSTANT( CoderFatalError );
	MW_REGISTER_LONG_CONSTANT( ModuleFatalError );
	MW_REGISTER_LONG_CONSTANT( DrawFatalError );
	MW_REGISTER_LONG_CONSTANT( ImageFatalError );
	MW_REGISTER_LONG_CONSTANT( WandFatalError );
/*	MW_REGISTER_LONG_CONSTANT( XServerFatalError ); */
	MW_REGISTER_LONG_CONSTANT( MonitorFatalError );
	MW_REGISTER_LONG_CONSTANT( RegistryFatalError );
	MW_REGISTER_LONG_CONSTANT( ConfigureFatalError );

	/***** FillRule *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedRule );
	MW_REGISTER_LONG_CONSTANT( EvenOddRule );
	MW_REGISTER_LONG_CONSTANT( NonZeroRule );

	/***** FilterTypes *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedFilter );
	MW_REGISTER_LONG_CONSTANT( PointFilter );
	MW_REGISTER_LONG_CONSTANT( BoxFilter );
	MW_REGISTER_LONG_CONSTANT( TriangleFilter );
	MW_REGISTER_LONG_CONSTANT( HermiteFilter );
	MW_REGISTER_LONG_CONSTANT( HanningFilter );
	MW_REGISTER_LONG_CONSTANT( HammingFilter );
	MW_REGISTER_LONG_CONSTANT( BlackmanFilter );
	MW_REGISTER_LONG_CONSTANT( GaussianFilter );
	MW_REGISTER_LONG_CONSTANT( QuadraticFilter );
	MW_REGISTER_LONG_CONSTANT( CubicFilter );
	MW_REGISTER_LONG_CONSTANT( CatromFilter );
	MW_REGISTER_LONG_CONSTANT( MitchellFilter );
	MW_REGISTER_LONG_CONSTANT( LanczosFilter );
	MW_REGISTER_LONG_CONSTANT( BesselFilter );
	MW_REGISTER_LONG_CONSTANT( SincFilter );

	/***** GravityType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedGravity );
	MW_REGISTER_LONG_CONSTANT( ForgetGravity );
	MW_REGISTER_LONG_CONSTANT( NorthWestGravity );
	MW_REGISTER_LONG_CONSTANT( NorthGravity );
	MW_REGISTER_LONG_CONSTANT( NorthEastGravity );
	MW_REGISTER_LONG_CONSTANT( WestGravity );
	MW_REGISTER_LONG_CONSTANT( CenterGravity );
	MW_REGISTER_LONG_CONSTANT( EastGravity );
	MW_REGISTER_LONG_CONSTANT( SouthWestGravity );
	MW_REGISTER_LONG_CONSTANT( SouthGravity );
	MW_REGISTER_LONG_CONSTANT( SouthEastGravity );
	MW_REGISTER_LONG_CONSTANT( StaticGravity );

	/***** ImageType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedType );
	MW_REGISTER_LONG_CONSTANT( BilevelType );
	MW_REGISTER_LONG_CONSTANT( GrayscaleType );
	MW_REGISTER_LONG_CONSTANT( GrayscaleMatteType );
	MW_REGISTER_LONG_CONSTANT( PaletteType );
	MW_REGISTER_LONG_CONSTANT( PaletteMatteType );
	MW_REGISTER_LONG_CONSTANT( TrueColorType );
	MW_REGISTER_LONG_CONSTANT( TrueColorMatteType );
	MW_REGISTER_LONG_CONSTANT( ColorSeparationType );
	MW_REGISTER_LONG_CONSTANT( ColorSeparationMatteType );
	MW_REGISTER_LONG_CONSTANT( OptimizeType );

	/***** InterlaceType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedInterlace );
	MW_REGISTER_LONG_CONSTANT( NoInterlace );
	MW_REGISTER_LONG_CONSTANT( LineInterlace );
	MW_REGISTER_LONG_CONSTANT( PlaneInterlace );
	MW_REGISTER_LONG_CONSTANT( PartitionInterlace );

	/***** LineCap *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedCap );
	MW_REGISTER_LONG_CONSTANT( ButtCap );
	MW_REGISTER_LONG_CONSTANT( RoundCap );
	MW_REGISTER_LONG_CONSTANT( SquareCap );

	/***** LineJoin *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedJoin );
	MW_REGISTER_LONG_CONSTANT( MiterJoin );
	MW_REGISTER_LONG_CONSTANT( RoundJoin );
	MW_REGISTER_LONG_CONSTANT( BevelJoin );

	/***** MagickEvaluateOperator *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( AddEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( AndEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( DivideEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( LeftShiftEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( MaxEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( MinEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( MultiplyEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( OrEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( RightShiftEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( SetEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( SubtractEvaluateOperator );
	MW_REGISTER_LONG_CONSTANT( XorEvaluateOperator );

	/***** MetricType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedMetric );
	MW_REGISTER_LONG_CONSTANT( MeanAbsoluteErrorMetric );
	MW_REGISTER_LONG_CONSTANT( MeanSquaredErrorMetric );
	MW_REGISTER_LONG_CONSTANT( PeakAbsoluteErrorMetric );
	MW_REGISTER_LONG_CONSTANT( PeakSignalToNoiseRatioMetric );
	MW_REGISTER_LONG_CONSTANT( RootMeanSquaredErrorMetric );

	/***** MontageMode *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedMode );
	MW_REGISTER_LONG_CONSTANT( FrameMode );
	MW_REGISTER_LONG_CONSTANT( UnframeMode );
	MW_REGISTER_LONG_CONSTANT( ConcatenateMode );

	/***** NoiseType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedNoise );
	MW_REGISTER_LONG_CONSTANT( UniformNoise );
	MW_REGISTER_LONG_CONSTANT( GaussianNoise );
	MW_REGISTER_LONG_CONSTANT( MultiplicativeGaussianNoise );
	MW_REGISTER_LONG_CONSTANT( ImpulseNoise );
	MW_REGISTER_LONG_CONSTANT( LaplacianNoise );
	MW_REGISTER_LONG_CONSTANT( PoissonNoise );

	/***** PaintMethod *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedMethod );
	MW_REGISTER_LONG_CONSTANT( PointMethod );
	MW_REGISTER_LONG_CONSTANT( ReplaceMethod );
	MW_REGISTER_LONG_CONSTANT( FloodfillMethod );
	MW_REGISTER_LONG_CONSTANT( FillToBorderMethod );
	MW_REGISTER_LONG_CONSTANT( ResetMethod );

	/***** PreviewType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedPreview );
	MW_REGISTER_LONG_CONSTANT( RotatePreview );
	MW_REGISTER_LONG_CONSTANT( ShearPreview );
	MW_REGISTER_LONG_CONSTANT( RollPreview );
	MW_REGISTER_LONG_CONSTANT( HuePreview );
	MW_REGISTER_LONG_CONSTANT( SaturationPreview );
	MW_REGISTER_LONG_CONSTANT( BrightnessPreview );
	MW_REGISTER_LONG_CONSTANT( GammaPreview );
	MW_REGISTER_LONG_CONSTANT( SpiffPreview );
	MW_REGISTER_LONG_CONSTANT( DullPreview );
	MW_REGISTER_LONG_CONSTANT( GrayscalePreview );
	MW_REGISTER_LONG_CONSTANT( QuantizePreview );
	MW_REGISTER_LONG_CONSTANT( DespecklePreview );
	MW_REGISTER_LONG_CONSTANT( ReduceNoisePreview );
	MW_REGISTER_LONG_CONSTANT( AddNoisePreview );
	MW_REGISTER_LONG_CONSTANT( SharpenPreview );
	MW_REGISTER_LONG_CONSTANT( BlurPreview );
	MW_REGISTER_LONG_CONSTANT( ThresholdPreview );
	MW_REGISTER_LONG_CONSTANT( EdgeDetectPreview );
	MW_REGISTER_LONG_CONSTANT( SpreadPreview );
	MW_REGISTER_LONG_CONSTANT( SolarizePreview );
	MW_REGISTER_LONG_CONSTANT( ShadePreview );
	MW_REGISTER_LONG_CONSTANT( RaisePreview );
	MW_REGISTER_LONG_CONSTANT( SegmentPreview );
	MW_REGISTER_LONG_CONSTANT( SwirlPreview );
	MW_REGISTER_LONG_CONSTANT( ImplodePreview );
	MW_REGISTER_LONG_CONSTANT( WavePreview );
	MW_REGISTER_LONG_CONSTANT( OilPaintPreview );
	MW_REGISTER_LONG_CONSTANT( CharcoalDrawingPreview );
	MW_REGISTER_LONG_CONSTANT( JPEGPreview );

	/***** RenderingIntent *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedIntent );
	MW_REGISTER_LONG_CONSTANT( SaturationIntent );
	MW_REGISTER_LONG_CONSTANT( PerceptualIntent );
	MW_REGISTER_LONG_CONSTANT( AbsoluteIntent );
	MW_REGISTER_LONG_CONSTANT( RelativeIntent );

	/***** ResolutionType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedResolution );
	MW_REGISTER_LONG_CONSTANT( PixelsPerInchResolution );
	MW_REGISTER_LONG_CONSTANT( PixelsPerCentimeterResolution );

	/***** ResourceType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedResource );
	MW_REGISTER_LONG_CONSTANT( AreaResource );
	MW_REGISTER_LONG_CONSTANT( DiskResource );
	MW_REGISTER_LONG_CONSTANT( FileResource );
	MW_REGISTER_LONG_CONSTANT( MapResource );
	MW_REGISTER_LONG_CONSTANT( MemoryResource );

	/***** StorageType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedPixel );
	MW_REGISTER_LONG_CONSTANT( CharPixel );
	MW_REGISTER_LONG_CONSTANT( ShortPixel );
	MW_REGISTER_LONG_CONSTANT( IntegerPixel );
	MW_REGISTER_LONG_CONSTANT( LongPixel );
	MW_REGISTER_LONG_CONSTANT( FloatPixel );
	MW_REGISTER_LONG_CONSTANT( DoublePixel );

	/***** StretchType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedStretch );
	MW_REGISTER_LONG_CONSTANT( NormalStretch );
	MW_REGISTER_LONG_CONSTANT( UltraCondensedStretch );
	MW_REGISTER_LONG_CONSTANT( ExtraCondensedStretch );
	MW_REGISTER_LONG_CONSTANT( CondensedStretch );
	MW_REGISTER_LONG_CONSTANT( SemiCondensedStretch );
	MW_REGISTER_LONG_CONSTANT( SemiExpandedStretch );
	MW_REGISTER_LONG_CONSTANT( ExpandedStretch );
	MW_REGISTER_LONG_CONSTANT( ExtraExpandedStretch );
	MW_REGISTER_LONG_CONSTANT( UltraExpandedStretch );
	MW_REGISTER_LONG_CONSTANT( AnyStretch );

	/***** StyleType *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedStyle );
	MW_REGISTER_LONG_CONSTANT( NormalStyle );
	MW_REGISTER_LONG_CONSTANT( ItalicStyle );
	MW_REGISTER_LONG_CONSTANT( ObliqueStyle );
	MW_REGISTER_LONG_CONSTANT( AnyStyle );

	/***** VirtualPixelMethod *****/
	MW_REGISTER_LONG_CONSTANT( UndefinedVirtualPixelMethod );
	MW_REGISTER_LONG_CONSTANT( ConstantVirtualPixelMethod );
	MW_REGISTER_LONG_CONSTANT( EdgeVirtualPixelMethod );
	MW_REGISTER_LONG_CONSTANT( MirrorVirtualPixelMethod );
	MW_REGISTER_LONG_CONSTANT( TileVirtualPixelMethod );

	return SUCCESS;
}
/* }}} */

/*********************************** defines ***********************************/
#define PRV_STR_LEN  512
/*******************************************************************************/
#define PRV_ARR_TO_INFO_TBL_ROW( heading_str, FunctionCall, separator, separator_len, heading )  \
{  \
	str_arr = (char **) FunctionCall;  \
	penul_idx = num_indices - 1;  \
\
	if ( str_arr != (char **) NULL && num_indices > 0 ) {  \
		for ( i = 0; i < num_indices; i++) {  \
			smart_str_appendl( &gen_str_list, str_arr[i], strlen( str_arr[i] ) );  \
			if ( i < penul_idx ) {  \
				smart_str_appendl( &gen_str_list, separator, separator_len );  \
			}  \
		}  \
		smart_str_0( &gen_str_list );  \
		snprintf( buffer, PRV_STR_LEN, "%s %s", pckg_name_constant, heading_str );  \
		php_info_print_table_row( 2, heading, gen_str_list.c );  \
		smart_str_free( &gen_str_list );  \
	}  \
	MW_FREE_MAGICK_MEM( char **, str_arr );  \
}
/*******************************************************************************/
#define PRV_PRINT_INFO( heading_str, data_str_var )  \
	snprintf( buffer, PRV_STR_LEN, "%s %s", pckg_name_constant, heading_str );  \
	php_info_print_table_row( 2, buffer, data_str_var );
/*******************************************************************************/

/* ZEND_MINFO_FUNCTION
 */
ZEND_MINFO_FUNCTION( magickwand )
{
	char buffer[ PRV_STR_LEN ], buffer_2[ PRV_STR_LEN ], *pckg_name_constant, *tmp_str_constant, **str_arr;
	unsigned long num_indices, penul_idx, i;
	smart_str gen_str_list = { 0 };

	php_info_print_table_start();

	pckg_name_constant = (char *) MagickGetPackageName();
	php_info_print_table_header( 2, "MagickWand Backend Library", pckg_name_constant );

	php_info_print_table_row( 2, "MagickWand Extension Version", MAGICKWAND_VERSION ); 

	PRV_PRINT_INFO( "support", "enabled" );

	tmp_str_constant = (char *) MagickGetVersion( (long *) NULL );
	PRV_PRINT_INFO( "version", tmp_str_constant );

	snprintf( buffer_2, PRV_STR_LEN, "%0.0f", MW_MaxRGB );
	PRV_PRINT_INFO( "MaxRGB", buffer_2 );
/*
	tmp_str_constant = (char *) MagickGetReleaseDate();
	PRV_PRINT_INFO( "release date", tmp_str_constant );

	tmp_str_constant = (char *) MagickGetQuantumDepth( (unsigned long *) NULL );
	PRV_PRINT_INFO( "quantum depth", tmp_str_constant );

	PRV_PRINT_INFO( "homepage", MagickHomeURL );
*/

	/************************* Get the list of ImageMagick configure options. *************************/
	PRV_ARR_TO_INFO_TBL_ROW(
		"configure options",
		MagickQueryConfigureOptions( "*", &num_indices ),
		" ", 1,
		buffer
	);

	/************************* Get the list of available image formats. *************************/
	PRV_ARR_TO_INFO_TBL_ROW(
		"supported image formats",
		MagickQueryFormats( "*", &num_indices ),
		", ", 2,
		buffer
	);

	/************************* Get a list of the available font names. *************************/
/* Commented out because users can't use them anyway -- they must use full path names to font
   files, in order to ensure PHP file restriction conformance
*/
/*
	PRV_ARR_TO_INFO_TBL_ROW(
		"available fonts",
		MagickQueryFonts( "*", &num_indices ),
		", ", 2,
		buffer
	);
*/
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/*
****************************************************************************************************
*************************         Generic Wand Exception Functions         *************************
****************************************************************************************************
*/
/*  The following functions can be used to determine if ANY wand contains an error
	-- should be VERY USEFUL
*/

/* {{{ proto array WandGetException( AnyWand wand )
*/
ZEND_FUNCTION( wandgetexception )
{
	MW_PRINT_DEBUG_INFO

	MW_DETERMINE_RSRC_TYPE_SET_ERROR_ARR();
}
/* }}} */

/* {{{ proto string WandGetExceptionString( AnyWand wand )
*/
ZEND_FUNCTION( wandgetexceptionstring )
{
	MW_PRINT_DEBUG_INFO

	MW_DETERMINE_RSRC_TYPE_SET_ERROR_STR();
}
/* }}} */

/* {{{ proto int WandGetExceptionType( AnyWand wand )
*/
ZEND_FUNCTION( wandgetexceptiontype )
{
	MW_PRINT_DEBUG_INFO

	MW_DETERMINE_RSRC_TYPE_SET_ERR_TYPE();
}
/* }}} */

/* {{{ proto bool WandHasException( AnyWand wand )
*/
ZEND_FUNCTION( wandhasexception )
{
	MW_PRINT_DEBUG_INFO

	MW_DETERMINE_RSRC_TYPE_SET_ERR_BOOL();
}
/* }}} */


/*
****************************************************************************************************
*************************             Wand Identity Functions              *************************
****************************************************************************************************
*/

/* {{{ proto bool IsDrawingWand( mixed var )
*/
ZEND_FUNCTION( isdrawingwand )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_VERIFY_TYPE_RET_BOOL( DrawingWand );
}
/* }}} */

/* {{{ proto bool IsMagickWand( mixed var )
*/
ZEND_FUNCTION( ismagickwand )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_VERIFY_TYPE_RET_BOOL( MagickWand );
}
/* }}} */

/* {{{ proto bool IsPixelIterator( mixed var )
*/
ZEND_FUNCTION( ispixeliterator )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_VERIFY_TYPE_RET_BOOL( PixelIterator );
}
/* }}} */

/* {{{ proto bool IsPixelWand( mixed var )
*/
ZEND_FUNCTION( ispixelwand )
{
	MW_PRINT_DEBUG_INFO

/*  Different macro is needed due to the existence of the le_PixelIteratorPixelWand
	registered resource type, which is just a PixelWand that Zend shouldn't destroy.
*/
	MW_GET_PixelWand_VERIFY_TYPE_RET_BOOL();
}
/* }}} */


/*
****************************************************************************************************
*************************     DrawingWand Functions     *************************
****************************************************************************************************
*/

/* {{{ proto void ClearDrawingWand( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( cleardrawingwand )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	ClearDrawingWand( drw_wnd );
}
/* }}} */

/* {{{ proto DrawingWand CloneDrawingWand( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( clonedrawingwand )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( DrawingWand, CloneDrawingWand( drw_wnd ) );
}
/* }}} */

/* {{{ proto void DestroyDrawingWand( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( destroydrawingwand )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_RSRC_DESTROY_POINTER( DrawingWand );
}
/* }}} */

/* {{{ proto void DrawAffine( DrawingWand drw_wnd, float sx, float sy, float rx, float ry, float tx, float ty )
*/
ZEND_FUNCTION( drawaffine )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	AffineMatrix *affine_mtrx;
	zval *drw_wnd_rsrc_zvl_p;
	double sx, sy, rx, ry, tx, ty;

	MW_GET_7_ARG( "rdddddd", &drw_wnd_rsrc_zvl_p, &sx, &sy, &rx, &ry, &tx, &ty );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_EMALLOC( AffineMatrix, affine_mtrx );

	affine_mtrx->sx = sx;
	affine_mtrx->sy = sy;

	affine_mtrx->rx = rx;
	affine_mtrx->ry = ry;

	affine_mtrx->tx = tx;
	affine_mtrx->ty = ty;

	DrawAffine( drw_wnd, affine_mtrx );

	efree( affine_mtrx );
}
/* }}} */

/* {{{ proto void DrawAnnotation( DrawingWand drw_wnd, float x, float y, string text )
*/
ZEND_FUNCTION( drawannotation )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;
	char *txt;
	int txt_len;

	MW_GET_5_ARG( "rdds", &drw_wnd_rsrc_zvl_p, &x, &y, &txt, &txt_len );

	MW_CHECK_PARAM_STR_LEN( txt_len );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawAnnotation( drw_wnd, x, y, (unsigned char *) txt );
}
/* }}} */

/* {{{ proto void DrawArc( DrawingWand drw_wnd, float sx, float sy, float ex, float ey, float sd, float ed )
*/
ZEND_FUNCTION( drawarc )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double  sx, sy, ex, ey, sd, ed;

	MW_GET_7_ARG( "rdddddd", &drw_wnd_rsrc_zvl_p, &sx, &sy, &ex, &ey, &sd, &ed );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawArc( drw_wnd, sx, sy, ex, ey, sd, ed );
}
/* }}} */

/* {{{ proto void DrawBezier( DrawingWand drw_wnd, array x_y_points_array )
*/
ZEND_FUNCTION( drawbezier )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_ARRAY_DRAW_POINTS( 6, DrawBezier );

/*	MW_GET_WAND_DRAW_POINTS( 7, DrawBezier );  */
}
/* }}} */

/* {{{ proto void DrawCircle( DrawingWand drw_wnd, float ox, float oy, float px, float py )
*/
ZEND_FUNCTION( drawcircle )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double ox, oy, px, py;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &ox, &oy, &px, &py );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawCircle( drw_wnd, ox, oy, px, py );
}
/* }}} */

/* {{{ proto void DrawColor( DrawingWand drw_wnd, float x, float y, int paint_method )
*/
ZEND_FUNCTION( drawcolor )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;
	long paint_method;

	MW_GET_4_ARG( "rddl", &drw_wnd_rsrc_zvl_p, &x, &y, &paint_method );

	MW_CHECK_CONSTANT( PaintMethod, paint_method );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawColor( drw_wnd, x, y, (PaintMethod) paint_method );
}
/* }}} */

/* {{{ proto void DrawComment( DrawingWand drw_wnd, string comment )
*/
ZEND_FUNCTION( drawcomment )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	char *comment;
	int comment_len;

	MW_GET_3_ARG( "rs", &drw_wnd_rsrc_zvl_p, &comment, &comment_len );

	MW_CHECK_PARAM_STR_LEN( comment_len );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawComment( drw_wnd, comment );
}
/* }}} */

/* {{{ proto bool DrawComposite( DrawingWand drw_wnd, int composite_operator, float x, float y, float width, float height, MagickWand mgck_wnd )
*/
ZEND_FUNCTION( drawcomposite )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	MagickWand *mgck_wnd;
	zval *drw_wnd_rsrc_zvl_p, *mgck_wnd_rsrc_zvl_p;
	long composite_op;
	double x, y, width, height;

	MW_GET_7_ARG( "rlddddr", &drw_wnd_rsrc_zvl_p, &composite_op, &x, &y, &width, &height, &mgck_wnd_rsrc_zvl_p );

	MW_CHECK_CONSTANT( CompositeOperator, composite_op );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( DrawComposite( drw_wnd, (CompositeOperator) composite_op, x, y, width, height, mgck_wnd ) );
}
/* }}} */

/* {{{ proto void DrawEllipse( DrawingWand drw_wnd, float ox, float oy, float rx, float ry, float start, float end )
*/
ZEND_FUNCTION( drawellipse )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double ox, oy, rx, ry, start, end;

	MW_GET_7_ARG( "rdddddd", &drw_wnd_rsrc_zvl_p, &ox, &oy, &rx, &ry, &start, &end );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawEllipse( drw_wnd, ox, oy, rx, ry, start, end );
}
/* }}} */

/* {{{ proto string DrawGetClipPath( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetclippath )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( DrawingWand, DrawGetClipPath );
}
/* }}} */

/* {{{ proto int DrawGetClipRule( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetcliprule )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetClipRule );
}
/* }}} */

/* {{{ proto int DrawGetClipUnits( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetclipunits )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetClipUnits );
}
/* }}} */

/* {{{ proto array DrawGetException( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetexception )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_ARR( DrawingWand );
}
/* }}} */

/* {{{ proto string DrawGetExceptionString( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetexceptionstring )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_STR( DrawingWand );
}
/* }}} */

/* {{{ proto int DrawGetExceptionType( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetexceptiontype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_TYPE( DrawingWand );
}
/* }}} */

/* {{{ proto float DrawGetFillAlpha( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfillalpha )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetFillAlpha );
}
/* }}} */

/* {{{ proto float DrawGetFillOpacity( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfillopacity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetFillOpacity );
}
/* }}} */

/* {{{ proto PixelWand DrawGetFillColor( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfillcolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_DO_VOID_FUNC_RET_PIXELWAND( DrawGetFillColor );
}
/* }}} */

/* {{{ proto int DrawGetFillRule( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfillrule )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetFillRule );
}
/* }}} */

/* {{{ proto string DrawGetFont( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfont )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( DrawingWand, DrawGetFont );
}
/* }}} */

/* {{{ proto string DrawGetFontFamily( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfontfamily )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( DrawingWand, DrawGetFontFamily );
}
/* }}} */

/* {{{ proto float DrawGetFontSize( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfontsize )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetFontSize );
}
/* }}} */

/* {{{ proto int DrawGetFontStretch( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfontstretch )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetFontStretch );
}
/* }}} */

/* {{{ proto int DrawGetFontStyle( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfontstyle )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetFontStyle );
}
/* }}} */

/* {{{ proto float DrawGetFontWeight( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetfontweight )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetFontWeight );
}
/* }}} */

/* {{{ proto int DrawGetGravity( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetgravity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetGravity );
}
/* }}} */

/* {{{ proto float DrawGetStrokeAlpha( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokealpha )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetStrokeAlpha );
}
/* }}} */

/* {{{ proto float DrawGetStrokeOpacity( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokeopacity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetStrokeOpacity );
}
/* }}} */

/* {{{ proto bool DrawGetStrokeAntialias( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokeantialias )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( DrawingWand, DrawGetStrokeAntialias );
}
/* }}} */

/* {{{ proto PixelWand DrawGetStrokeColor( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokecolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_DO_VOID_FUNC_RET_PIXELWAND( DrawGetStrokeColor );
}
/* }}} */

/* {{{ proto array DrawGetStrokeDashArray( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokedasharray )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE_ARR( DrawingWand, DrawGetStrokeDashArray );
}
/* }}} */

/* {{{ proto float DrawGetStrokeDashOffset( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokedashoffset )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetStrokeDashOffset );
}
/* }}} */

/* {{{ proto int DrawGetStrokeLineCap( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokelinecap )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetStrokeLineCap );
}
/* }}} */

/* {{{ proto int DrawGetStrokeLineJoin( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokelinejoin )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetStrokeLineJoin );
}
/* }}} */

/* {{{ proto float DrawGetStrokeMiterLimit( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokemiterlimit )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetStrokeMiterLimit );
}
/* }}} */

/* {{{ proto float DrawGetStrokeWidth( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetstrokewidth )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( DrawingWand, DrawGetStrokeWidth );
}
/* }}} */

/* {{{ proto int DrawGetTextAlignment( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgettextalignment )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetTextAlignment );
}
/* }}} */

/* {{{ proto bool DrawGetTextAntialias( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgettextantialias )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( DrawingWand, DrawGetTextAntialias );
}
/* }}} */

/* {{{ proto int DrawGetTextDecoration( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgettextdecoration )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( DrawingWand, DrawGetTextDecoration );
}
/* }}} */

/* {{{ proto string DrawGetTextEncoding( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgettextencoding )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( DrawingWand, DrawGetTextEncoding );
}
/* }}} */

/* {{{ proto string DrawGetVectorGraphics( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgetvectorgraphics )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( DrawingWand, DrawGetVectorGraphics );
}
/* }}} */

/* {{{ proto PixelWand DrawGetTextUnderColor( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawgettextundercolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_DO_VOID_FUNC_RET_PIXELWAND( DrawGetTextUnderColor );
}
/* }}} */

/* {{{ proto void DrawLine( DrawingWand drw_wnd, float sx, float sy, float ex, float ey )
*/
ZEND_FUNCTION( drawline )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double sx, sy, ex, ey;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &sx, &sy, &ex, &ey );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawLine( drw_wnd, sx, sy, ex, ey );
}
/* }}} */

/* {{{ proto void DrawMatte( DrawingWand drw_wnd, float x, float y, int paint_method )
*/
ZEND_FUNCTION( drawmatte )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;
	long paint_method;

	MW_GET_4_ARG( "rddl", &drw_wnd_rsrc_zvl_p, &x, &y, &paint_method );

	MW_CHECK_CONSTANT( PaintMethod, paint_method );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawMatte( drw_wnd, x, y, (PaintMethod) paint_method );
}
/* }}} */

/* {{{ proto void DrawPathClose( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpathclose )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathClose( drw_wnd );
}
/* }}} */

/* {{{ proto void DrawPathCurveToAbsolute( DrawingWand drw_wnd, float x1, float y1, float x2, float y2, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetoabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x2, y2, x, y;

	MW_GET_7_ARG( "rdddddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x2, &y2, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToAbsolute( drw_wnd, x1, y1, x2, y2, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToRelative( DrawingWand drw_wnd, float x1, float y1, float x2, float y2, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetorelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x2, y2, x, y;

	MW_GET_7_ARG( "rdddddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x2, &y2, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToRelative( drw_wnd, x1, y1, x2, y2, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToQuadraticBezierAbsolute( DrawingWand drw_wnd, float x1, float y1, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetoquadraticbezierabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x, y;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToQuadraticBezierAbsolute( drw_wnd, x1, y1, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToQuadraticBezierRelative( DrawingWand drw_wnd, float x1, float y1, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetoquadraticbezierrelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x, y;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToQuadraticBezierRelative( drw_wnd, x1, y1, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToQuadraticBezierSmoothAbsolute( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetoquadraticbeziersmoothabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToQuadraticBezierSmoothAbsolute( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToQuadraticBezierSmoothRelative( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetoquadraticbeziersmoothrelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToQuadraticBezierSmoothRelative( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToSmoothAbsolute( DrawingWand drw_wnd, float x2, float y2, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetosmoothabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x2, y2, x, y;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &x2, &y2, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToSmoothAbsolute( drw_wnd, x2, y2, x, y );
}
/* }}} */

/* {{{ proto void DrawPathCurveToSmoothRelative( DrawingWand drw_wnd, float x2, float y2, float x, float y )
*/
ZEND_FUNCTION( drawpathcurvetosmoothrelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x2, y2, x, y;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &x2, &y2, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathCurveToSmoothRelative( drw_wnd, x2, y2, x, y );
}
/* }}} */

/* {{{ proto void DrawPathEllipticArcAbsolute( DrawingWand drw_wnd, float rx, float ry, float x_axis_rotation, bool large_arc_flag, bool sweep_flag, float x, float y )
*/
ZEND_FUNCTION( drawpathellipticarcabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double rx, ry, x_axis_rotation, x, y;
	zend_bool do_large_arc, do_sweep;

	MW_GET_8_ARG( "rdddbbdd", &drw_wnd_rsrc_zvl_p, &rx, &ry, &x_axis_rotation, &do_large_arc, &do_sweep, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathEllipticArcAbsolute(
		drw_wnd,
		rx, ry,
		x_axis_rotation,
		MW_MK_MGCK_BOOL(do_large_arc),
		MW_MK_MGCK_BOOL(do_sweep),
		x, y
	);
}
/* }}} */

/* {{{ proto void DrawPathEllipticArcRelative( DrawingWand drw_wnd, float rx, float ry, float x_axis_rotation, bool large_arc_flag, bool sweep_flag, float x, float y )
*/
ZEND_FUNCTION( drawpathellipticarcrelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double rx, ry, x_axis_rotation, x, y;
	zend_bool do_large_arc, do_sweep;

	MW_GET_8_ARG( "rdddbbdd", &drw_wnd_rsrc_zvl_p, &rx, &ry, &x_axis_rotation, &do_large_arc, &do_sweep, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathEllipticArcRelative(
		drw_wnd,
		rx, ry,
		x_axis_rotation,
		MW_MK_MGCK_BOOL(do_large_arc),
		MW_MK_MGCK_BOOL(do_sweep),
		x, y
	);
}
/* }}} */

/* {{{ proto void DrawPathFinish( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpathfinish )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathFinish( drw_wnd );
}
/* }}} */

/* {{{ proto void DrawPathLineToAbsolute( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpathlinetoabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathLineToAbsolute( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPathLineToRelative( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpathlinetorelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathLineToRelative( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPathLineToHorizontalAbsolute( DrawingWand drw_wnd, float x )
*/
ZEND_FUNCTION( drawpathlinetohorizontalabsolute )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawPathLineToHorizontalAbsolute );
}
/* }}} */

/* {{{ proto void DrawPathLineToHorizontalRelative( DrawingWand drw_wnd, float x )
*/
ZEND_FUNCTION( drawpathlinetohorizontalrelative )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawPathLineToHorizontalRelative );
}
/* }}} */

/* {{{ proto void DrawPathLineToVerticalAbsolute( DrawingWand drw_wnd, float y )
*/
ZEND_FUNCTION( drawpathlinetoverticalabsolute )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawPathLineToVerticalAbsolute );
}
/* }}} */

/* {{{ proto void DrawPathLineToVerticalRelative( DrawingWand drw_wnd, float y )
*/
ZEND_FUNCTION( drawpathlinetoverticalrelative )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawPathLineToVerticalRelative );
}
/* }}} */

/* {{{ proto void DrawPathMoveToAbsolute( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpathmovetoabsolute )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathMoveToAbsolute( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPathMoveToRelative( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpathmovetorelative )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathMoveToRelative( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPathStart( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpathstart )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPathStart( drw_wnd );
}
/* }}} */

/* {{{ proto void DrawPoint( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawpoint )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPoint( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawPolygon( DrawingWand drw_wnd, array x_y_points_array )
*/
ZEND_FUNCTION( drawpolygon )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_ARRAY_DRAW_POINTS( 6, DrawPolygon );

/*	MW_GET_WAND_DRAW_POINTS( 7, DrawPolygon );  */
}
/* }}} */

/* {{{ proto void DrawPolyline( DrawingWand drw_wnd, array x_y_points_array )
*/
ZEND_FUNCTION( drawpolyline )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_ARRAY_DRAW_POINTS( 4, DrawPolyline );

/*	MW_GET_WAND_DRAW_POINTS( 5, DrawPolyline );  */
}
/* }}} */

/* {{{ proto void DrawPopClipPath( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpopclippath )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPopClipPath( drw_wnd );
}
/* }}} */

/* {{{ proto void DrawPopDefs( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpopdefs )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPopDefs( drw_wnd );
}
/* }}} */

/* {{{ proto bool DrawPopPattern( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpoppattern )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( DrawingWand, DrawPopPattern );
}
/* }}} */

/* {{{ proto void DrawPushClipPath( DrawingWand drw_wnd, string clip_path_id )
*/
ZEND_FUNCTION( drawpushclippath )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	char *clip_path_id;
	int clip_path_id_len;

	MW_GET_3_ARG( "rs", &drw_wnd_rsrc_zvl_p, &clip_path_id, &clip_path_id_len );

	MW_CHECK_PARAM_STR_LEN( clip_path_id_len );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPushClipPath( drw_wnd, clip_path_id );
}
/* }}} */

/* {{{ proto void DrawPushDefs( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawpushdefs )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawPushDefs( drw_wnd );
}
/* }}} */

/* {{{ proto bool DrawPushPattern( DrawingWand drw_wnd, string pattern_id, float x, float y, float width, float height )
*/
ZEND_FUNCTION( drawpushpattern )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	char *pttrn_id;
	int pttrn_id_len;
	double x, y, width, height;

	MW_GET_7_ARG( "rsdddd", &drw_wnd_rsrc_zvl_p, &pttrn_id, &pttrn_id_len, &x, &y, &width, &height );

	MW_CHECK_PARAM_STR_LEN( pttrn_id_len );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( DrawPushPattern( drw_wnd, pttrn_id, x, y, width, height ) );
}
/* }}} */

/* {{{ proto void DrawRectangle( DrawingWand drw_wnd, float x1, float y1, float x2, float y2 )
*/
ZEND_FUNCTION( drawrectangle )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x2, y2;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x2, &y2 );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawRectangle( drw_wnd, x1, y1, x2, y2 );
}
/* }}} */

/* {{{ proto bool DrawRender( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( drawrender )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( DrawingWand, DrawRender );
}
/* }}} */

/* {{{ proto void DrawRotate( DrawingWand drw_wnd, float degrees )
*/
ZEND_FUNCTION( drawrotate )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawRotate );
}
/* }}} */

/* {{{ proto void DrawRoundRectangle( DrawingWand drw_wnd, float x1, float y1, float x2, float y2, float rx, float ry )
*/
ZEND_FUNCTION( drawroundrectangle )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x2, y2, rx, ry;

	MW_GET_7_ARG( "rdddddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x2, &y2, &rx, &ry );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawRoundRectangle( drw_wnd, x1, y1, x2, y2, rx, ry );
}
/* }}} */

/* {{{ proto void DrawScale( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawscale )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawScale( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto bool DrawSetClipPath( DrawingWand drw_wnd, string clip_path )
*/
ZEND_FUNCTION( drawsetclippath )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( DrawingWand, DrawSetClipPath );
}
/* }}} */

/* {{{ proto void DrawSetClipRule( DrawingWand drw_wnd, int fill_rule )
*/
ZEND_FUNCTION( drawsetcliprule )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( FillRule, DrawSetClipRule );
}
/* }}} */

/* {{{ proto void DrawSetClipUnits( DrawingWand drw_wnd, int clip_path_units )
*/
ZEND_FUNCTION( drawsetclipunits )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( ClipPathUnits, DrawSetClipUnits );
}
/* }}} */

/* {{{ proto void DrawSetFillAlpha( DrawingWand drw_wnd, float fill_opacity )
*/
ZEND_FUNCTION( drawsetfillalpha )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( DrawingWand, DrawSetFillAlpha );
}
/* }}} */

/* {{{ proto void DrawSetFillOpacity( DrawingWand drw_wnd, float fill_opacity )
*/
ZEND_FUNCTION( drawsetfillopacity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( DrawingWand, DrawSetFillOpacity );
}
/* }}} */

/* {{{ proto void DrawSetFillColor( DrawingWand drw_wnd, mixed fill_pxl_wnd )
*/
ZEND_FUNCTION( drawsetfillcolor )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	PixelWand *fill_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							DrawingWand, drw_wnd,
							"a DrawingWand resource, a fill color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, fill_pxl_wnd, is_script_pxl_wnd );

	DrawSetFillColor( drw_wnd, fill_pxl_wnd );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		fill_pxl_wnd = DestroyPixelWand( fill_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool DrawSetFillPatternURL( DrawingWand drw_wnd, string fill_url )
*/
ZEND_FUNCTION( drawsetfillpatternurl )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( DrawingWand, DrawSetFillPatternURL );
}
/* }}} */

/* {{{ proto void DrawSetFillRule( DrawingWand drw_wnd, int fill_rule )
*/
ZEND_FUNCTION( drawsetfillrule )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( FillRule, DrawSetFillRule );
}
/* }}} */

/* {{{ proto bool DrawSetFont( DrawingWand drw_wnd, string font_file )
*/
ZEND_FUNCTION( drawsetfont )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	char *font_filename;
	int font_filename_len;

	MW_GET_3_ARG( "rs", &drw_wnd_rsrc_zvl_p, &font_filename, &font_filename_len );

	MW_CHECK_PARAM_STR_LEN( font_filename_len );

	MW_FILE_IS_READABLE( font_filename );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( DrawSetFont( drw_wnd, font_filename ) );
}
/* }}} */

/* {{{ proto bool DrawSetFontFamily( DrawingWand drw_wnd, string font_family )
*/
ZEND_FUNCTION( drawsetfontfamily )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( DrawingWand, DrawSetFontFamily );
}
/* }}} */

/* {{{ proto void DrawSetFontSize( DrawingWand drw_wnd, float pointsize )
*/
ZEND_FUNCTION( drawsetfontsize )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawSetFontSize );
}
/* }}} */

/* {{{ proto void DrawSetFontStretch( DrawingWand drw_wnd, int stretch_type )
*/
ZEND_FUNCTION( drawsetfontstretch )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( StretchType, DrawSetFontStretch );
}
/* }}} */

/* {{{ proto void DrawSetFontStyle( DrawingWand drw_wnd, int style_type )
*/
ZEND_FUNCTION( drawsetfontstyle )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( StyleType, DrawSetFontStyle );
}
/* }}} */

/* {{{ proto void DrawSetFontWeight( DrawingWand drw_wnd, float font_weight )
*/
ZEND_FUNCTION( drawsetfontweight )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	long font_weight;

	MW_GET_2_ARG( "rl", &drw_wnd_rsrc_zvl_p, &font_weight );

	MW_CHECK_VALUE_RANGE( font_weight, 100, 900 );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawSetFontWeight( drw_wnd, (unsigned long) font_weight );
}
/* }}} */

/* {{{ proto void DrawSetGravity( DrawingWand drw_wnd, int gravity_type )
*/
ZEND_FUNCTION( drawsetgravity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( GravityType, DrawSetGravity );
}
/* }}} */

/* {{{ proto void DrawSetStrokeAlpha( DrawingWand drw_wnd, float stroke_opacity )
*/
ZEND_FUNCTION( drawsetstrokealpha )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( DrawingWand, DrawSetStrokeAlpha );
}
/* }}} */

/* {{{ proto void DrawSetStrokeOpacity( DrawingWand drw_wnd, float stroke_opacity )
*/
ZEND_FUNCTION( drawsetstrokeopacity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( DrawingWand, DrawSetStrokeOpacity );
}
/* }}} */

/* {{{ proto void DrawSetStrokeAntialias( DrawingWand drw_wnd [, bool stroke_antialias] )
*/
ZEND_FUNCTION( drawsetstrokeantialias )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	zend_bool anti_alias = TRUE;

	MW_GET_2_ARG( "r|b", &drw_wnd_rsrc_zvl_p, &anti_alias );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawSetStrokeAntialias( drw_wnd, MW_MK_MGCK_BOOL(anti_alias) );
}
/* }}} */

/* {{{ proto void DrawSetStrokeColor( DrawingWand drw_wnd, mixed strokecolor_pxl_wnd )
*/
ZEND_FUNCTION( drawsetstrokecolor )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	PixelWand *strokecolor_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							DrawingWand, drw_wnd,
							"a DrawingWand resource, a strokecolor color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, strokecolor_pxl_wnd, is_script_pxl_wnd );

	DrawSetStrokeColor( drw_wnd, strokecolor_pxl_wnd );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		strokecolor_pxl_wnd = DestroyPixelWand( strokecolor_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool DrawSetStrokeDashArray( DrawingWand drw_wnd [, array dash_array] )
*/
ZEND_FUNCTION( drawsetstrokedasharray )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p, *zvl_arr = (zval *) NULL, **zvl_pp_element;
	unsigned long num_elements, i = 0;
	double *dash_arr;
	HashPosition pos;

	MW_GET_2_ARG( "r|a!", &drw_wnd_rsrc_zvl_p, &zvl_arr );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	if ( zvl_arr == (zval *) NULL || Z_TYPE_P( zvl_arr ) == IS_NULL ) {
		MW_BOOL_FUNC_RETVAL_BOOL( DrawSetStrokeDashArray( drw_wnd, 0, (double *) NULL ) );
	}
	else {
		num_elements = (unsigned long) zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

		if ( num_elements == 0 ) {
			MW_BOOL_FUNC_RETVAL_BOOL( DrawSetStrokeDashArray( drw_wnd, 0, (double *) NULL ) );
		}
		else {
			MW_ARR_ECALLOC( double, dash_arr, num_elements );

			MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element ) {
				convert_to_double_ex( zvl_pp_element );
				dash_arr[ i++ ] = Z_DVAL_PP( zvl_pp_element );
			}
			MW_BOOL_FUNC_RETVAL_BOOL( DrawSetStrokeDashArray( drw_wnd, num_elements, dash_arr ) );
			efree( dash_arr );
		}
	}
}
/* }}} */

/* {{{ proto void DrawSetStrokeDashOffset( DrawingWand drw_wnd, float dash_offset )
*/
ZEND_FUNCTION( drawsetstrokedashoffset )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawSetStrokeDashOffset );
}
/* }}} */

/* {{{ proto void DrawSetStrokeLineCap( DrawingWand drw_wnd, int line_cap )
*/
ZEND_FUNCTION( drawsetstrokelinecap )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( LineCap, DrawSetStrokeLineCap );
}
/* }}} */

/* {{{ proto void DrawSetStrokeLineJoin( DrawingWand drw_wnd, int line_join )
*/
ZEND_FUNCTION( drawsetstrokelinejoin )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( LineJoin, DrawSetStrokeLineJoin );
}
/* }}} */

/* {{{ proto void DrawSetStrokeMiterLimit( DrawingWand drw_wnd, float miterlimit )
*/
ZEND_FUNCTION( drawsetstrokemiterlimit )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double miterlimit;

	MW_GET_2_ARG( "rd", &drw_wnd_rsrc_zvl_p, &miterlimit );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawSetStrokeMiterLimit( drw_wnd, (unsigned long) miterlimit );
}
/* }}} */

/* {{{ proto bool DrawSetStrokePatternURL( DrawingWand drw_wnd, string stroke_url )
*/
ZEND_FUNCTION( drawsetstrokepatternurl )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( DrawingWand, DrawSetStrokePatternURL );
}
/* }}} */

/* {{{ proto void DrawSetStrokeWidth( DrawingWand drw_wnd, float stroke_width )
*/
ZEND_FUNCTION( drawsetstrokewidth )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawSetStrokeWidth );
}
/* }}} */

/* {{{ proto void DrawSetTextAlignment( DrawingWand drw_wnd, int align_type )
*/
ZEND_FUNCTION( drawsettextalignment )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( AlignType, DrawSetTextAlignment );
}
/* }}} */

/* {{{ proto void DrawSetTextAntialias( DrawingWand drw_wnd [, bool text_antialias] )
*/
ZEND_FUNCTION( drawsettextantialias )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	zend_bool anti_alias = TRUE;

	MW_GET_2_ARG( "r|b", &drw_wnd_rsrc_zvl_p, &anti_alias );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawSetTextAntialias( drw_wnd, MW_MK_MGCK_BOOL(anti_alias) );
}
/* }}} */

/* {{{ proto void DrawSetTextDecoration( DrawingWand drw_wnd, int decoration_type )
*/
ZEND_FUNCTION( drawsettextdecoration )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_ENUM( DecorationType, DrawSetTextDecoration );
}
/* }}} */

/* {{{ proto void DrawSetTextEncoding( DrawingWand drw_wnd, string encoding )
*/
ZEND_FUNCTION( drawsettextencoding )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	char *enc_str;
	int enc_str_len;

	MW_GET_3_ARG( "rs", &drw_wnd_rsrc_zvl_p, &enc_str, &enc_str_len );

	MW_CHECK_PARAM_STR_LEN( enc_str_len );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawSetTextEncoding( drw_wnd, enc_str );
}
/* }}} */

/* {{{ proto void DrawSetTextUnderColor( DrawingWand drw_wnd, mixed undercolor_pxl_wnd )
*/
ZEND_FUNCTION( drawsettextundercolor )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	PixelWand *undercolor_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							DrawingWand, drw_wnd,
							"a DrawingWand resource and a undercolor PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, undercolor_pxl_wnd, is_script_pxl_wnd );

	DrawSetTextUnderColor( drw_wnd, undercolor_pxl_wnd );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		undercolor_pxl_wnd = DestroyPixelWand( undercolor_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool DrawSetVectorGraphics( DrawingWand drw_wnd, string vector_graphics )
*/
ZEND_FUNCTION( drawsetvectorgraphics )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	char *str;
	int str_len;

	MW_GET_3_ARG( "r|s", &drw_wnd_rsrc_zvl_p, &str, &str_len );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );
	MW_BOOL_FUNC_RETVAL_BOOL( DrawSetVectorGraphics( drw_wnd, ( str_len > 0 ? str : "" ) ) );
}
/* }}} */

/* {{{ proto void DrawSkewX( DrawingWand drw_wnd, float degrees )
*/
ZEND_FUNCTION( drawskewx )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawSkewX );
}
/* }}} */

/* {{{ proto void DrawSkewY( DrawingWand drw_wnd, float degrees )
*/
ZEND_FUNCTION( drawskewy )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_DRAWINGWAND_SET_DOUBLE( DrawSkewY );
}
/* }}} */

/* {{{ proto void DrawTranslate( DrawingWand drw_wnd, float x, float y )
*/
ZEND_FUNCTION( drawtranslate )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &drw_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawTranslate( drw_wnd, x, y );
}
/* }}} */

/* {{{ proto void DrawSetViewbox( DrawingWand drw_wnd, float x1, float y1, float x2, float y2 )
*/
ZEND_FUNCTION( drawsetviewbox )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;
	double x1, y1, x2, y2;

	MW_GET_5_ARG( "rdddd", &drw_wnd_rsrc_zvl_p, &x1, &y1, &x2, &y2 );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	DrawSetViewbox(
		drw_wnd,
		(unsigned long) x1, (unsigned long) y1,
		(unsigned long) x2, (unsigned long) y2
	);
}
/* }}} */

/* {{{ proto DrawingWand NewDrawingWand( void )
*/
ZEND_FUNCTION( newdrawingwand )
{
	MW_PRINT_DEBUG_INFO

	MW_SET_RET_RSRC_FROM_FUNC( DrawingWand, NewDrawingWand() );
}
/* }}} */

/* {{{ proto void PopDrawingWand( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( popdrawingwand )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	PopDrawingWand( drw_wnd );
}
/* }}} */

/* {{{ proto void PushDrawingWand( DrawingWand drw_wnd )
*/
ZEND_FUNCTION( pushdrawingwand )
{
	MW_PRINT_DEBUG_INFO

	DrawingWand *drw_wnd;
	zval *drw_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	PushDrawingWand( drw_wnd );
}
/* }}} */


/*
****************************************************************************************************
*************************     MagickWand Functions     *************************
****************************************************************************************************
*/

/* {{{ proto void ClearMagickWand( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( clearmagickwand )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	ClearMagickWand( mgck_wnd );
}
/* }}} */

/* {{{ proto MagickWand NewMagickWand( void )
*/
ZEND_FUNCTION( newmagickwand )
{
	MW_PRINT_DEBUG_INFO

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, NewMagickWand() );
}
/* }}} */

/* {{{ proto MagickWand CloneMagickWand( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( clonemagickwand )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, CloneMagickWand( mgck_wnd ) );
}
/* }}} */

/* {{{ proto void DestroyMagickWand( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( destroymagickwand )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_RSRC_DESTROY_POINTER( MagickWand );
}
/* }}} */

/* {{{ proto bool MagickAdaptiveThresholdImage( MagickWand mgck_wnd, float width, float height, int offset )
*/
ZEND_FUNCTION( magickadaptivethresholdimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double width, height;
	long offset;

	MW_GET_4_ARG( "rddl", &mgck_wnd_rsrc_zvl_p, &width, &height, &offset );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickAdaptiveThresholdImage( mgck_wnd, (unsigned long) width, (unsigned long) height, offset ) );
}
/* }}} */

/* {{{ proto bool MagickAddImage( MagickWand mgck_wnd, MagickWand add_wand )
*/
ZEND_FUNCTION( magickaddimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *get_wnd, *add_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *add_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &add_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, add_wnd, &add_wnd_rsrc_zvl_p );

	get_wnd = (MagickWand *) MagickGetImage( add_wnd );

	if ( get_wnd == (MagickWand *) NULL ) {

		MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	add_wnd, MagickGetException,
											"unable to retrieve the current active image of the 2nd MagickWand "  \
											"resource argument; unable to perform the copy operation"
		);

		return;
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickAddImage( mgck_wnd, get_wnd ) );

	get_wnd = DestroyMagickWand( get_wnd );
}
/* }}} */

/* {{{ proto bool MagickAddImages( MagickWand mgck_wnd, MagickWand add_wand )
*/
ZEND_FUNCTION( magickaddimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *add_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *add_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &add_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, add_wnd, &add_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickAddImage( mgck_wnd, add_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickAddNoiseImage( MagickWand mgck_wnd, int noise_type )
*/
ZEND_FUNCTION( magickaddnoiseimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( NoiseType, MagickAddNoiseImage );
}
/* }}} */

/* {{{ proto bool MagickAffineTransformImage( MagickWand mgck_wnd, DrawingWand drw_wnd )
*/
ZEND_FUNCTION( magickaffinetransformimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	DrawingWand *drw_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *drw_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickAffineTransformImage( mgck_wnd, drw_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickAnnotateImage( MagickWand mgck_wnd, DrawingWand drw_wnd, float x, float y, float angle, string text )
*/
ZEND_FUNCTION( magickannotateimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	DrawingWand *drw_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *drw_wnd_rsrc_zvl_p;
	double x, y, angle;
	char *txt;
	int txt_len;

	MW_GET_7_ARG( "rrddds", &mgck_wnd_rsrc_zvl_p, &drw_wnd_rsrc_zvl_p, &x, &y, &angle, &txt, &txt_len );

	MW_CHECK_PARAM_STR_LEN( txt_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickAnnotateImage( mgck_wnd, drw_wnd, x, y, angle, txt ) );
}
/* }}} */

/* {{{ proto MagickWand MagickAppendImages( MagickWand mgck_wnd [, bool stack_vertical] )
*/
ZEND_FUNCTION( magickappendimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	zend_bool stack_vertical = FALSE;

	MW_GET_2_ARG( "r|b", &mgck_wnd_rsrc_zvl_p, &stack_vertical );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickAppendImages( mgck_wnd, MW_MK_MGCK_BOOL(stack_vertical) ) );
}
/* }}} */

/* {{{ proto MagickWand MagickAverageImages( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickaverageimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickAverageImages( mgck_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickBlackThresholdImage( MagickWand mgck_wnd, mixed threshold_pxl_wnd )
*/
ZEND_FUNCTION( magickblackthresholdimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *threshold_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a threshold color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, threshold_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickBlackThresholdImage( mgck_wnd, threshold_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		threshold_pxl_wnd = DestroyPixelWand( threshold_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickBlurImage( MagickWand mgck_wnd, float radius, float sigma [, int channel_type] )
*/
ZEND_FUNCTION( magickblurimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma;
	long channel = -1;

	MW_GET_4_ARG( "rdd|l", &mgck_wnd_rsrc_zvl_p, &radius, &sigma, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickBlurImage( mgck_wnd, radius, sigma ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickBlurImageChannel( mgck_wnd, (ChannelType) channel, radius, sigma ) );
	}
}
/* }}} */

/* {{{ proto bool MagickBorderImage( MagickWand mgck_wnd, mixed bordercolor, float width, float height )
*/
ZEND_FUNCTION( magickborderimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *bordercolor_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double width, height;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 4),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a bordercolor PixelWand resource "  \
							"(or ImageMagick color string), and the desired border width and height" );

	convert_to_double_ex( zvl_pp_args_arr[2] );
	width =    Z_DVAL_PP( zvl_pp_args_arr[2] );

	convert_to_double_ex( zvl_pp_args_arr[3] );
	height =   Z_DVAL_PP( zvl_pp_args_arr[3] );

	if ( width < 1 && height < 1 ) {
		MW_SPIT_FATAL_ERR( "cannot create an image border smaller than 1 pixel in width and height" );
		efree( zvl_pp_args_arr );
		return;
	}

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, bordercolor_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickBorderImage( mgck_wnd, bordercolor_pxl_wnd, (unsigned long) width, (unsigned long) height ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		bordercolor_pxl_wnd = DestroyPixelWand( bordercolor_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickCharcoalImage( MagickWand mgck_wnd, float radius, float sigma )
*/
ZEND_FUNCTION( magickcharcoalimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &radius, &sigma );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickCharcoalImage( mgck_wnd, radius, sigma ) );
}
/* }}} */

/* {{{ proto bool MagickChopImage( MagickWand mgck_wnd, float width, float height, int x, int y )
*/
ZEND_FUNCTION( magickchopimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double width, height;
	long x, y;

	MW_GET_5_ARG( "rddll", &mgck_wnd_rsrc_zvl_p, &width, &height, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickChopImage( mgck_wnd, (unsigned long) width, (unsigned long) height, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickClipImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickclipimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickClipImage );
}
/* }}} */

/* {{{ proto bool MagickClipPathImage( MagickWand mgck_wnd, string pathname, bool inside )
*/
ZEND_FUNCTION( magickclippathimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *path_name;
	int path_name_len;
	zend_bool clip_inside;

	MW_GET_4_ARG( "rsb", &mgck_wnd_rsrc_zvl_p, &path_name, &path_name_len, &clip_inside );

	MW_CHECK_PARAM_STR_LEN( path_name_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickClipPathImage( mgck_wnd, path_name, MW_MK_MGCK_BOOL(clip_inside) ) );
}
/* }}} */

/* {{{ proto MagickWand MagickCoalesceImages( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickcoalesceimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickCoalesceImages( mgck_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickColorFloodfillImage( MagickWand mgck_wnd, mixed fillcolor_pxl_wnd, float fuzz, mixed bordercolor_pxl_wnd, int x, int y )
*/
ZEND_FUNCTION( magickcolorfloodfillimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *fillcolor_pxl_wnd, *bordercolor_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd, is_script_pxl_wnd_2;
	double fuzz;
	long x, y;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 6),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a fill color PixelWand resource (or ImageMagick "  \
							"color string), a fuzz value, a bordercolor PixelWand resource (or "  \
							"ImageMagick color string), and the x and y ordinates of the starting pixel" );

	convert_to_double_ex( zvl_pp_args_arr[2] );
	fuzz =     Z_DVAL_PP( zvl_pp_args_arr[2] );

	convert_to_long_ex( zvl_pp_args_arr[4] );
	x =      Z_LVAL_PP( zvl_pp_args_arr[4] );

	convert_to_long_ex( zvl_pp_args_arr[5] );
	y =      Z_LVAL_PP( zvl_pp_args_arr[5] );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, fillcolor_pxl_wnd, is_script_pxl_wnd );

	if ( Z_TYPE_PP( zvl_pp_args_arr[3] ) == IS_RESOURCE ) {

		if (  (   MW_FETCH_RSRC( PixelWand, bordercolor_pxl_wnd, zvl_pp_args_arr[3] ) == MagickFalse
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, bordercolor_pxl_wnd, zvl_pp_args_arr[3] ) == MagickFalse )
			|| IsPixelWand( bordercolor_pxl_wnd ) == MagickFalse )
		{
			MW_SPIT_FATAL_ERR( "invalid resource type as argument #4; a PixelWand resource is required" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				fillcolor_pxl_wnd = DestroyPixelWand( fillcolor_pxl_wnd );
			}
			return;
		}
		is_script_pxl_wnd_2 = 1;
	}
	else {
		if ( Z_TYPE_PP( zvl_pp_args_arr[3] ) == IS_NULL ) {
			is_script_pxl_wnd_2 = 1;
			bordercolor_pxl_wnd = (PixelWand *) NULL;
		}
		else {
			is_script_pxl_wnd_2 = 0;

			bordercolor_pxl_wnd = (PixelWand *) NewPixelWand();

			if ( bordercolor_pxl_wnd == (PixelWand *) NULL ) {
				MW_SPIT_FATAL_ERR( "unable to create necessary PixelWand" );
				efree( zvl_pp_args_arr );
				if ( !is_script_pxl_wnd ) {
					fillcolor_pxl_wnd = DestroyPixelWand( fillcolor_pxl_wnd );
				}
				return;
			}
			convert_to_string_ex( zvl_pp_args_arr[3] );

			if ( Z_STRLEN_PP( zvl_pp_args_arr[3] ) > 0 ) {

				if ( PixelSetColor( bordercolor_pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[3] ) ) == MagickFalse ) {

					MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	bordercolor_pxl_wnd, PixelGetException,
														"could not set PixelWand to desired fill color"
					);

					bordercolor_pxl_wnd = DestroyPixelWand( bordercolor_pxl_wnd );
					efree( zvl_pp_args_arr );

					if ( !is_script_pxl_wnd ) {
						fillcolor_pxl_wnd = DestroyPixelWand( fillcolor_pxl_wnd );
					}

					return;
				}
			}
		}
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickColorFloodfillImage( mgck_wnd, fillcolor_pxl_wnd, fuzz, bordercolor_pxl_wnd, x, y ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		fillcolor_pxl_wnd = DestroyPixelWand( fillcolor_pxl_wnd );
	}
	if ( !is_script_pxl_wnd_2 ) {
		bordercolor_pxl_wnd = DestroyPixelWand( bordercolor_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickColorizeImage( MagickWand mgck_wnd, mixed colorize_pxl_wnd, mixed opacity_pxl_wnd )
*/
ZEND_FUNCTION( magickcolorizeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *colorize_pxl_wnd, *opacity_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd, is_script_pxl_wnd_2;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 3),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource and a tint color PixelWand resource (or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, colorize_pxl_wnd, is_script_pxl_wnd );

	if ( Z_TYPE_PP( zvl_pp_args_arr[2] ) == IS_RESOURCE ) {

		if (  (   MW_FETCH_RSRC( PixelWand, opacity_pxl_wnd, zvl_pp_args_arr[2] ) == MagickFalse
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, opacity_pxl_wnd, zvl_pp_args_arr[2] ) == MagickFalse )
			|| IsPixelWand( opacity_pxl_wnd ) == MagickFalse )
		{
			MW_SPIT_FATAL_ERR( "invalid resource type as argument #3; a PixelWand resource is required" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				colorize_pxl_wnd = DestroyPixelWand( colorize_pxl_wnd );
			}
			return;
		}
		is_script_pxl_wnd_2 = 1;
	}
	else {
		is_script_pxl_wnd_2 = 0;

		opacity_pxl_wnd = (PixelWand *) NewPixelWand();

		if ( opacity_pxl_wnd == (PixelWand *) NULL ) {
			MW_SPIT_FATAL_ERR( "unable to create necessary PixelWand" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				colorize_pxl_wnd = DestroyPixelWand( colorize_pxl_wnd );
			}
			return;
		}
		convert_to_string_ex( zvl_pp_args_arr[2] );

		if ( Z_STRLEN_PP( zvl_pp_args_arr[2] ) > 0 ) {

			if ( PixelSetColor( opacity_pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[2] ) ) == MagickFalse ) {

				MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	opacity_pxl_wnd, PixelGetException,
													"could not set PixelWand to desired fill color"
				);

				opacity_pxl_wnd = DestroyPixelWand( opacity_pxl_wnd );
				efree( zvl_pp_args_arr );

				if ( !is_script_pxl_wnd ) {
					colorize_pxl_wnd = DestroyPixelWand( colorize_pxl_wnd );
				}

				return;
			}
		}
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickColorizeImage( mgck_wnd, colorize_pxl_wnd, opacity_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		colorize_pxl_wnd = DestroyPixelWand( colorize_pxl_wnd );
	}
	if ( !is_script_pxl_wnd_2 ) {
		opacity_pxl_wnd = DestroyPixelWand( opacity_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto MagickWand MagickCombineImages( MagickWand mgck_wnd, int channel_type )
*/
ZEND_FUNCTION( magickcombineimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long channel;

	MW_GET_2_ARG( "rl", &mgck_wnd_rsrc_zvl_p, &channel );

	MW_CHECK_CONSTANT( ChannelType, channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickCombineImages( mgck_wnd, (ChannelType) channel ) );
}
/* }}} */

/* {{{ proto bool MagickCommentImage( MagickWand mgck_wnd, string comment )
*/
ZEND_FUNCTION( magickcommentimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( MagickWand, MagickCommentImage );
}
/* }}} */

/* {{{ proto array MagickCompareImages( MagickWand mgck_wnd, MagickWand reference_wnd, int metric_type [, int channel_type] )
*/
ZEND_FUNCTION( magickcompareimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *ref_wnd, *comp_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *ref_wnd_rsrc_zvl_p;
	long metric, channel = -1;
	double distortion;
	int comp_wnd_rsrc_id;

	MW_GET_4_ARG( "rrl|l", &mgck_wnd_rsrc_zvl_p, &ref_wnd_rsrc_zvl_p, &metric, &channel );

	MW_CHECK_CONSTANT( MetricType, metric );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, ref_wnd, &ref_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		comp_wnd = (MagickWand *) MagickCompareImages( mgck_wnd, ref_wnd, (MetricType) metric, &distortion );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		comp_wnd = (MagickWand *) MagickCompareImageChannels( mgck_wnd, ref_wnd, (ChannelType) channel, (MetricType) metric, &distortion );
	}

	if ( comp_wnd == (MagickWand *) NULL ) {
		RETURN_FALSE;
	}

	if ( MW_ZEND_REGISTER_RESOURCE( MagickWand, comp_wnd, (zval *) NULL, &comp_wnd_rsrc_id ) == MagickFalse ) {
		comp_wnd = DestroyMagickWand( comp_wnd );
		RETURN_FALSE;
	}

	array_init( return_value );

	MW_SET_2_RET_ARR_VALS( add_next_index_resource( return_value, comp_wnd_rsrc_id ),
						   add_next_index_double( return_value, distortion ) );
}
/* }}} */

/* {{{ proto bool MagickCompositeImage( MagickWand mgck_wnd, MagickWand composite_wnd, int composite_operator, int x, int y )
*/
ZEND_FUNCTION( magickcompositeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *composite_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *composite_wnd_rsrc_zvl_p;
	long compose, x, y;

	MW_GET_5_ARG( "rrlll", &mgck_wnd_rsrc_zvl_p, &composite_wnd_rsrc_zvl_p, &compose, &x, &y );

	MW_CHECK_CONSTANT( CompositeOperator, compose );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, composite_wnd, &composite_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickCompositeImage( mgck_wnd, composite_wnd, (CompositeOperator) compose, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickConstituteImage( MagickWand mgck_wnd, float columns, float rows, string map, int storage_type, array pixel_array )
*/
ZEND_FUNCTION( magickconstituteimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_arr, **zvl_pp_element;
	double columns, rows;
	char *map, *c;
	int map_len, i, pxl_arr_len;
	long php_storage;
	StorageType storage;
	HashPosition pos;

	MW_GET_7_ARG( "rddsla",
				  &mgck_wnd_rsrc_zvl_p,
				  &columns, &rows,
				  &map, &map_len,
				  &php_storage,
				  &zvl_arr );

	MW_CHECK_PARAM_STR_LEN( map_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	for ( i = 0; i < map_len; i++ ) {
		c = &(map[i]);

		if ( !( *c == 'A' || *c == 'a' ||
				*c == 'B' || *c == 'b' ||
				*c == 'C' || *c == 'c' ||
				*c == 'G' || *c == 'g' ||
				*c == 'I' || *c == 'i' ||
				*c == 'K' || *c == 'k' ||
				*c == 'M' || *c == 'm' ||
				*c == 'O' || *c == 'o' ||
				*c == 'P' || *c == 'p' ||
				*c == 'R' || *c == 'r' ||
				*c == 'Y' || *c == 'y' ) ) {
			MW_SPIT_FATAL_ERR( "map parameter string can only contain the letters A B C G I K M O P R Y; see docs for details" );
			return;
		}
	}

	pxl_arr_len = zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

	if ( pxl_arr_len == 0 ) {
		MW_SPIT_FATAL_ERR( "pixel array parameter was empty" );
		return;
	}

	i = (int) ( columns * rows * map_len );

	if ( pxl_arr_len != i ) {
		zend_error( MW_E_ERROR, "%s(): actual size of pixel array (%d) does not match expected size (%u)",
								get_active_function_name( TSRMLS_C ), pxl_arr_len, i );
		return;
	}

	storage = (StorageType) php_storage;

	i = 0;

#define PRV_SET_PIXELS_ARR_RET_BOOL_1( type )  \
{   type *pxl_arr;  \
	MW_ARR_ECALLOC( type, pxl_arr, pxl_arr_len );  \
	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element )  \
	{  \
		convert_to_double_ex( zvl_pp_element );  \
		pxl_arr[ i++ ] = (type) Z_DVAL_PP( zvl_pp_element );  \
	}  \
	MW_BOOL_FUNC_RETVAL_BOOL(  \
		MagickConstituteImage( mgck_wnd,  \
							   (unsigned long) columns, (unsigned long) rows,  \
							   map,  \
							   storage,  \
							   (unsigned char *) pxl_arr ) );  \
	efree( pxl_arr );  \
}

	switch ( storage ) {
		 case CharPixel		: PRV_SET_PIXELS_ARR_RET_BOOL_1( unsigned char  );  break;
		 case ShortPixel	: PRV_SET_PIXELS_ARR_RET_BOOL_1( unsigned short );  break;
		 case IntegerPixel	: PRV_SET_PIXELS_ARR_RET_BOOL_1( unsigned int   );  break;
		 case LongPixel		: PRV_SET_PIXELS_ARR_RET_BOOL_1( unsigned long  );  break;
		 case FloatPixel	: PRV_SET_PIXELS_ARR_RET_BOOL_1( float          );  break;
		 case DoublePixel	: PRV_SET_PIXELS_ARR_RET_BOOL_1( double         );  break;

		 default : MW_SPIT_FATAL_ERR( "Invalid StorageType argument supplied to function" );
				   return;
	}
}
/* }}} */

/* {{{ proto bool MagickContrastImage( MagickWand mgck_wnd, bool sharpen )
*/
ZEND_FUNCTION( magickcontrastimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	zend_bool sharpen = FALSE;

	MW_GET_2_ARG( "r|b", &mgck_wnd_rsrc_zvl_p, &sharpen );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickContrastImage( mgck_wnd, MW_MK_MGCK_BOOL(sharpen) ) );
}
/* }}} */

/* {{{ proto bool MagickConvolveImage( MagickWand mgck_wnd, array kernel_array [, int channel_type] )
*/
ZEND_FUNCTION( magickconvolveimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_arr, **zvl_pp_element;
	long channel = -1;
	unsigned long order, i = 0;
	double num_elements, *kernel_arr;
	HashPosition pos;

	MW_GET_3_ARG( "ra|l", &mgck_wnd_rsrc_zvl_p, &zvl_arr, &channel );

	num_elements = (double) zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

	if ( num_elements < 1 ) {
		MW_SPIT_FATAL_ERR( "the array parameter was empty" );
		return;
	}

	order = (unsigned long) sqrt( num_elements );

	if ( pow( (double) order, 2 ) != num_elements ) {
		MW_SPIT_FATAL_ERR( "array parameter length was not square; array must contain a square number amount of doubles" );
		return;
	}

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_ARR_ECALLOC( double, kernel_arr, num_elements );

	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element ) {
		convert_to_double_ex( zvl_pp_element );

		kernel_arr[i++] = Z_DVAL_PP( zvl_pp_element );
	}

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickConvolveImage( mgck_wnd, order, kernel_arr ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickConvolveImageChannel( mgck_wnd, (ChannelType) channel, order, kernel_arr ) );
	}

	efree( kernel_arr );
}
/* }}} */

/* {{{ proto bool MagickCropImage( MagickWand mgck_wnd, float width, float height, int x, int y )
*/
ZEND_FUNCTION( magickcropimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double width, height;
	long x, y;

	MW_GET_5_ARG( "rddll", &mgck_wnd_rsrc_zvl_p, &width, &height, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickCropImage( mgck_wnd, (unsigned long) width, (unsigned long) height, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickCycleColormapImage( MagickWand mgck_wnd, int num_positions )
*/
ZEND_FUNCTION( magickcyclecolormapimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long displace;

	MW_GET_2_ARG( "rl", &mgck_wnd_rsrc_zvl_p, &displace );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickCycleColormapImage( mgck_wnd, displace ) );
}
/* }}} */

/* {{{ proto MagickWand MagickDeconstructImages( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickdeconstructimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickDeconstructImages( mgck_wnd ) );
}
/* }}} */

/* {{{ proto string MagickDescribeImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickdescribeimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( MagickWand, MagickDescribeImage );
}
/* }}} */

/* {{{ proto bool MagickDespeckleImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickdespeckleimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickDespeckleImage );
}
/* }}} */

/* {{{ proto bool MagickDrawImage( MagickWand mgck_wnd, DrawingWand drw_wnd )
*/
ZEND_FUNCTION( magickdrawimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	DrawingWand *drw_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *drw_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &drw_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickDrawImage( mgck_wnd, drw_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickEchoImageBlob( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickechoimageblob )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long img_idx;
	char *orig_filename = (char *) NULL, *orig_img_format;
	int filename_valid;
	unsigned char *blob;
	size_t blob_len = 0;
	MagickBooleanType img_had_format;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );
	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Saves the current active image index, as well as performs a shortcut check for any exceptions,
	   as they will occur if mgck_wnd contains no images
	*/
	img_idx = (long) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_IMAGE_FORMAT( mgck_wnd, img_idx, img_had_format, orig_img_format, "MagickEchoImageBlob" );

	orig_filename = (char *) MagickGetImageFilename( mgck_wnd );
	filename_valid = !(orig_filename == (char *) NULL || *orig_filename == '\0');

	if ( filename_valid ) {
		MagickSetImageFilename( mgck_wnd, "" );
	}

	blob = (unsigned char *) MagickGetImageBlob( mgck_wnd, &blob_len );

	if ( blob == (unsigned char *) NULL || *blob == '\0' ) {

		if ( mgck_wnd->exception.severity == UndefinedException ) {
			zend_error( MW_E_ERROR, "%s(): an unknown error occurred; the image BLOB to be output was empty "  \
									"-- a possible cause may be that the format or filename was not set",
									get_active_function_name(TSRMLS_C) );
		}
		else {
			char *mw_err_str;
			ExceptionType mw_severity;

			mw_err_str = (char *) MagickGetException( mgck_wnd, &mw_severity );
			if ( mw_err_str == (char *) NULL || *mw_err_str == '\0' ) {
				zend_error( MW_E_ERROR, "%s(): an unknown exception occurred", get_active_function_name(TSRMLS_C) );
			}
			else {
				zend_error( MW_E_ERROR, "%s(): a MagickWand exception occurred: %s",
										get_active_function_name(TSRMLS_C), mw_err_str );
			}
			MW_FREE_MAGICK_MEM( char *, mw_err_str );
		}

		MW_FREE_MAGICK_MEM( unsigned char *, blob );

		MW_FREE_MAGICK_MEM( char *, orig_img_format );
		MW_FREE_MAGICK_MEM( char *, orig_filename );

		return;
	}
	else {
		php_write( blob, blob_len TSRMLS_CC);
		RETVAL_TRUE;
	}

	MW_FREE_MAGICK_MEM( unsigned char *, blob );

	if ( filename_valid ) {
		MagickSetImageFilename( mgck_wnd, orig_filename );
	}

	/* Check if current image had a format before function started */
	if ( img_had_format == MagickFalse ) {

		MW_DEBUG_NOTICE_EX( "Attempting to reset image #%d's image format", img_idx );

		/* If not, it must have been changed to the MagickWand's format, so set it back (probably to nothing) */
		if ( MagickSetImageFormat( mgck_wnd, orig_img_format ) == MagickFalse ) {

			MW_DEBUG_NOTICE_EX(	"FAILURE: could not reset image #%d's image format", img_idx );

			/* C API could not reset set the current image's format back to its original state:
			   output error, with reason
			*/
			MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
													"unable to set the image at MagickWand index %d back to "  \
													"its original image format",
													img_idx
			);
		}
	}

	MW_FREE_MAGICK_MEM( char *, orig_img_format );
	MW_FREE_MAGICK_MEM( char *, orig_filename );
}
/* }}} */

/* {{{ proto bool MagickEchoImagesBlob( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickechoimagesblob )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long img_idx;
	char *orig_filename = (char *) NULL;
	int filename_valid;
	unsigned char *blob;
	size_t blob_len = 0;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );
	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Saves the current active image index, as well as performs a shortcut check for any exceptions,
	   as they will occur if mgck_wnd contains no images
	*/
	img_idx = (long) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_WAND_FORMAT( mgck_wnd, "MagickEchoImagesBlob" );

	orig_filename = (char *) MagickGetFilename( mgck_wnd );
	filename_valid = !(orig_filename == (char *) NULL || *orig_filename == '\0');

	if ( filename_valid ) {
		MagickSetFilename( mgck_wnd, "" );
	}

	blob = (unsigned char *) MagickGetImagesBlob( mgck_wnd, &blob_len );

	if ( blob == (unsigned char *) NULL || *blob == '\0' ) {

		if ( mgck_wnd->exception.severity == UndefinedException ) {
			zend_error( MW_E_ERROR, "%s(): an unknown error occurred; the image BLOB to be output was empty",
									get_active_function_name(TSRMLS_C) );
		}
		else {
			char *mw_err_str;
			ExceptionType mw_severity;

			mw_err_str = (char *) MagickGetException( mgck_wnd, &mw_severity );
			if ( mw_err_str == (char *) NULL || *mw_err_str == '\0' ) {
				zend_error( MW_E_ERROR, "%s(): an unknown exception occurred", get_active_function_name(TSRMLS_C) );
			}
			else {
				zend_error( MW_E_ERROR, "%s(): a MagickWand exception occurred: %s",
										get_active_function_name(TSRMLS_C), mw_err_str );
			}
			MW_FREE_MAGICK_MEM( char *, mw_err_str );
		}
		
		MW_FREE_MAGICK_MEM( unsigned char *, blob );
		MW_FREE_MAGICK_MEM( char *, orig_filename );

		return;
	}
	else {
		php_write( blob, blob_len TSRMLS_CC);
		RETVAL_TRUE;
	}

	MW_FREE_MAGICK_MEM( unsigned char *, blob );

	if ( filename_valid ) {
		MagickSetFilename( mgck_wnd, orig_filename );
	}

	MW_FREE_MAGICK_MEM( char *, orig_filename );
}
/* }}} */

/* {{{ proto bool MagickEdgeImage( MagickWand mgck_wnd, float radius )
*/
ZEND_FUNCTION( magickedgeimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickEdgeImage );
}
/* }}} */

/* {{{ proto bool MagickEmbossImage( MagickWand mgck_wnd, float radius, float sigma )
*/
ZEND_FUNCTION( magickembossimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &radius, &sigma );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickEmbossImage( mgck_wnd, radius, sigma ) );
}
/* }}} */

/* {{{ proto bool MagickEnhanceImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickenhanceimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickEnhanceImage );
}
/* }}} */

/* {{{ proto bool MagickEqualizeImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickequalizeimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickEqualizeImage );
}
/* }}} */

/* {{{ proto bool MagickEvaluateImage( MagickWand mgck_wnd, int evaluate_op, float constant [, int channel_type] )
*/
ZEND_FUNCTION( magickevaluateimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long eval_op, channel = -1;
	double constant;

	MW_GET_4_ARG( "rld|l", &mgck_wnd_rsrc_zvl_p, &eval_op, &constant, &channel );

	MW_CHECK_CONSTANT( MagickEvaluateOperator, eval_op );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickEvaluateImage( mgck_wnd, (MagickEvaluateOperator) eval_op, constant ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickEvaluateImageChannel( mgck_wnd, (ChannelType) channel, (MagickEvaluateOperator) eval_op, constant ) );
	}
}
/* }}} */

/* {{{ proto MagickWand MagickFlattenImages( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickflattenimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickFlattenImages( mgck_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickFlipImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickflipimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickFlipImage );
}
/* }}} */

/* {{{ proto bool MagickFlopImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickflopimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickFlopImage );
}
/* }}} */

/* {{{ proto bool MagickFrameImage( MagickWand mgck_wnd, mixed matte_pxl_wnd, float width, float height, int inner_bevel, int outer_bevel )
*/
ZEND_FUNCTION( magickframeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *matte_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double width, height;
	long inner_bevel, outer_bevel;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 6),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a matte color PixelWand resource (or "  \
							"ImageMagick color string), the desired frame width and height, "  \
							"and the desired inner and outer bevel sizes" );

	convert_to_double_ex( zvl_pp_args_arr[2] );
	width =    Z_DVAL_PP( zvl_pp_args_arr[2] );

	convert_to_double_ex( zvl_pp_args_arr[3] );
	height =   Z_DVAL_PP( zvl_pp_args_arr[3] );
/*
	if ( width < 1 && height < 1 ) {
		MW_SPIT_FATAL_ERR( "cannot create a frame smaller than 1 pixel in width and height" );
		efree( zvl_pp_args_arr );
		return;
	}
*/
	convert_to_long_ex(      zvl_pp_args_arr[4] );
	inner_bevel = Z_LVAL_PP( zvl_pp_args_arr[4] );

	convert_to_long_ex(      zvl_pp_args_arr[5] );
	outer_bevel = Z_LVAL_PP( zvl_pp_args_arr[5] );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, matte_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL(
		MagickFrameImage( mgck_wnd, matte_pxl_wnd, (unsigned long) width, (unsigned long) height, inner_bevel, outer_bevel )
	);

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		matte_pxl_wnd = DestroyPixelWand( matte_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto MagickWand MagickFxImage( MagickWand mgck_wnd, string expression [, int channel_type] )
*/
ZEND_FUNCTION( magickfximage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *expression;
	int expression_len;
	long channel = -1;

	MW_GET_4_ARG( "rs|l", &mgck_wnd_rsrc_zvl_p, &expression, &expression_len, &channel );

	MW_CHECK_PARAM_STR_LEN( expression_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickFxImage( mgck_wnd, expression ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickFxImageChannel( mgck_wnd, (ChannelType) channel, expression ) );
	}
}
/* }}} */

/* {{{ proto bool MagickGammaImage( MagickWand mgck_wnd, float gamma [, int channel_type] )
*/
ZEND_FUNCTION( magickgammaimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long channel = -1;
	double gamma;

	MW_GET_3_ARG( "rd|l", &mgck_wnd_rsrc_zvl_p, &gamma, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickGammaImage( mgck_wnd, gamma ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickGammaImageChannel( mgck_wnd, (ChannelType) channel, gamma ) );
	}
}
/* }}} */

/* {{{ proto bool MagickGaussianBlurImage( MagickWand mgck_wnd, float radius, float sigma [, int channel_type] )
*/
ZEND_FUNCTION( magickgaussianblurimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma;
	long channel = -1;

	MW_GET_4_ARG( "rdd|l", &mgck_wnd_rsrc_zvl_p, &radius, &sigma, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickGaussianBlurImage( mgck_wnd, radius, sigma ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickGaussianBlurImageChannel( mgck_wnd, (ChannelType) channel, radius, sigma ) );
	}
}
/* }}} */

/* {{{ proto string MagickGetCopyright( void )
*/
ZEND_FUNCTION( magickgetcopyright )
{
	MW_PRINT_DEBUG_INFO

	char *copyright;

/* No freeing of copyright after this call, because string returned is a constant string, ie "..."
*/
	copyright = (char *) MagickGetCopyright();

	RETURN_STRING( copyright, 1 );
}
/* }}} */

/* {{{ proto array MagickGetException( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetexception )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_ARR( MagickWand );
}
/* }}} */

/* {{{ proto string MagickGetExceptionString( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetexceptionstring )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_STR( MagickWand );
}
/* }}} */

/* {{{ proto int MagickGetExceptionType( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetexceptiontype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_TYPE( MagickWand );
}
/* }}} */

/* {{{ proto string MagickGetFilename( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetfilename )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( MagickWand,  MagickGetFilename );
}
/* }}} */

/* {{{ proto string MagickGetFormat( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetformat )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *img_format;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	img_format = (char *) MagickGetFormat( mgck_wnd );

	if ( img_format == (char *) NULL || *img_format == '\0' || *img_format == '*' ) {

		if ( (mgck_wnd)->exception.severity == UndefinedException ) {
			RETVAL_MW_EMPTY_STRING();
		}
		else {
			RETVAL_FALSE;
		}
	}
	else {
		RETVAL_STRING( img_format, 1 );
	}

	MW_FREE_MAGICK_MEM( char *, img_format );
}
/* }}} */

/* {{{ proto string MagickGetHomeURL( void )
*/
ZEND_FUNCTION( magickgethomeurl )
{
	MW_PRINT_DEBUG_INFO

	char *ret_str;

	ret_str = (char *) MagickGetHomeURL();

	if ( ret_str == (char *) NULL || *ret_str == '\0' ) {
		RETURN_MW_EMPTY_STRING();
	}

	RETVAL_STRING( ret_str, 1 );

	MW_FREE_MAGICK_MEM( char *, ret_str );
}
/* }}} */

/* {{{ proto MagickWand MagickGetImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickGetImage( mgck_wnd ) );
}
/* }}} */

/* {{{ proto PixelWand MagickGetImageBackgroundColor( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagebackgroundcolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_DO_BOOL_FUNC_RET_PIXELWAND( MagickGetImageBackgroundColor );
}
/* }}} */

/* {{{ proto string MagickGetImageBlob( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageblob )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *orig_filename = (char *) NULL, *orig_img_format;
	long img_idx;
	int filename_valid;
	size_t blob_len = 0;
	MagickBooleanType img_had_format;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );
	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Saves the current active image index, as well as performs a shortcut check for any exceptions,
	   as they will occur if mgck_wnd contains no images
	*/
	img_idx = (long) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_IMAGE_FORMAT( mgck_wnd, img_idx, img_had_format, orig_img_format, "MagickGetImageBlob" );

	orig_filename = (char *) MagickGetImageFilename( mgck_wnd );
	filename_valid = !(orig_filename == (char *) NULL || *orig_filename == '0');

	if ( filename_valid ) {
		MagickSetImageFilename( mgck_wnd, "" );
	}

	MW_FUNC_RETVAL_STRING_L( mgck_wnd, MagickGetImageBlob( mgck_wnd, &blob_len ), blob_len );

	if ( filename_valid ) {
		MagickSetImageFilename( mgck_wnd, orig_filename );
	}

	/* Check if current image had a format before function started */
	if ( img_had_format == MagickFalse ) {

		MW_DEBUG_NOTICE_EX( "Attempting to reset image #%d's image format", img_idx );

		/* If not, it must have been changed to the MagickWand's format, so set it back (probably to nothing) */
		if ( MagickSetImageFormat( mgck_wnd, orig_img_format ) == MagickFalse ) {

			MW_DEBUG_NOTICE_EX(	"FAILURE: could not reset image #%d's image format", img_idx );

			/* C API could not reset set the current image's format back to its original state:
			   output error, with reason
			*/
			MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
													"unable to set the image at MagickWand index %d back to "  \
													"its original image format",
													img_idx
			);
		}
	}

	MW_FREE_MAGICK_MEM( char *, orig_img_format );
	MW_FREE_MAGICK_MEM( char *, orig_filename );
}
/* }}} */

/* {{{ proto string MagickGetImagesBlob( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagesblob )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *orig_filename = (char *) NULL;
	long img_idx;
	int filename_valid;
	size_t blob_len = 0;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );
	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Saves the current active image index, as well as performs a shortcut check for any exceptions,
	   as they will occur if mgck_wnd contains no images
	*/
	img_idx = (long) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_WAND_FORMAT( mgck_wnd, "MagickGetImagesBlob" );

	orig_filename = (char *) MagickGetFilename( mgck_wnd );
	filename_valid = !(orig_filename == (char *) NULL || *orig_filename == '0');

	if ( filename_valid ) {
		MagickSetFilename( mgck_wnd, "" );
	}

	MW_FUNC_RETVAL_STRING_L( mgck_wnd, MagickGetImagesBlob( mgck_wnd, &blob_len ), blob_len );

	if ( filename_valid ) {
		MagickSetFilename( mgck_wnd, orig_filename );
	}

	MW_FREE_MAGICK_MEM( char *, orig_filename );
}
/* }}} */

/* {{{ proto array MagickGetImageBluePrimary( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageblueprimary )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageBluePrimary( mgck_wnd, &x, &y ), x, y );
}
/* }}} */

/* {{{ proto PixelWand MagickGetImageBorderColor( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagebordercolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_DO_BOOL_FUNC_RET_PIXELWAND( MagickGetImageBorderColor );
}
/* }}} */

/* {{{ proto array MagickGetImageChannelMean( MagickWand mgck_wnd, int channel_type )
*/
ZEND_FUNCTION( magickgetimagechannelmean )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long channel;
	double mean, standard_deviation;

	MW_GET_2_ARG( "rl", &mgck_wnd_rsrc_zvl_p, &channel );

	MW_CHECK_CONSTANT( ChannelType, channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageChannelMean( mgck_wnd, (ChannelType) channel, &mean, &standard_deviation ),
								   mean, standard_deviation );
}
/* }}} */

/* {{{ proto PixelWand MagickGetImageColormapColor( MagickWand mgck_wnd, float index )
*/
ZEND_FUNCTION( magickgetimagecolormapcolor )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *colormap_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double index;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &index );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	colormap_wnd = (PixelWand *) NewPixelWand();

	if ( MagickGetImageColormapColor( mgck_wnd, (unsigned long) index, colormap_wnd ) == MagickTrue ) {
		MW_SET_RET_RSRC( PixelWand, colormap_wnd );
	}
	else {
		colormap_wnd = DestroyPixelWand( colormap_wnd );
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto float MagickGetImageColors( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagecolors )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageColors );
}
/* }}} */

/* {{{ proto int MagickGetImageColorspace( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagecolorspace )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageColorspace );
}
/* }}} */

/* {{{ proto int MagickGetImageCompose( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagecompose )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageCompose );
}
/* }}} */

/* {{{ proto int MagickGetImageCompression( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagecompression )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageCompression );
}
/* }}} */

/* {{{ proto float MagickGetImageCompressionQuality( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagecompressionquality )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageCompressionQuality );
}
/* }}} */

/* {{{ proto float MagickGetImageDelay( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagedelay )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageDelay );
}
/* }}} */

/* {{{ proto float MagickGetImageDepth( MagickWand mgck_wnd [, int channel_type] )
*/
ZEND_FUNCTION( magickgetimagedepth )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long channel = -1;
	unsigned long channel_depth;

	MW_GET_2_ARG( "r|l", &mgck_wnd_rsrc_zvl_p, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		channel_depth = (unsigned long) MagickGetImageDepth( mgck_wnd );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		channel_depth = (unsigned long) MagickGetImageChannelDepth( mgck_wnd, (ChannelType) channel );
	}

	if ( mgck_wnd->exception.severity == UndefinedException ) {
		RETURN_LONG( (long) channel_depth );
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto float MagickGetImageDistortion( MagickWand mgck_wnd, MagickWand ref_wnd, MetricType metric [, ChannelType channel] )
*/
ZEND_FUNCTION( magickgetimagedistortion )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *ref_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *ref_wnd_rsrc_zvl_p;
	long metric, channel = -1;
	MagickBooleanType magick_return;
	double distortion;

	MW_GET_4_ARG( "rrl|l", &mgck_wnd_rsrc_zvl_p, &ref_wnd_rsrc_zvl_p, &metric, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );
	MW_GET_POINTER_FROM_RSRC( MagickWand, ref_wnd,  &ref_wnd_rsrc_zvl_p  );

	MW_CHECK_CONSTANT( MetricType, metric );

	if ( channel == -1 ) {
		magick_return = MagickGetImageDistortion( mgck_wnd, ref_wnd, (MetricType) metric, &distortion );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		magick_return = (MagickBooleanType) MagickGetImageChannelDistortion(	mgck_wnd, ref_wnd,
																				(ChannelType) channel, (MetricType) metric,
																				&distortion );
	}

	if ( magick_return == MagickTrue ) {
		RETURN_DOUBLE( distortion );
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int MagickGetImageDispose( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagedispose )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageDispose );
}
/* }}} */

/* {{{ proto array MagickGetImageExtrema( MagickWand mgck_wnd [, int channel_type] )
*/
ZEND_FUNCTION( magickgetimageextrema )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long channel = -1;
	unsigned long minima, maxima;

	MW_GET_2_ARG( "r|l", &mgck_wnd_rsrc_zvl_p, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageExtrema( mgck_wnd, &minima, &maxima ),
									   (double) minima, (double) maxima );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageChannelExtrema( mgck_wnd, (ChannelType) channel, &minima, &maxima ),
									   (double) minima, (double) maxima );
	}
}
/* }}} */

/* {{{ proto string MagickGetImageFilename( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagefilename )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( MagickWand,  MagickGetImageFilename );
}
/* }}} */

/* {{{ proto string MagickGetImageFormat( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageformat )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *img_format;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	img_format = (char *) MagickGetImageFormat( mgck_wnd );

	if ( img_format == (char *) NULL || *img_format == '\0' || *img_format == '*' ) {

		if ( (mgck_wnd)->exception.severity == UndefinedException ) {
			RETVAL_MW_EMPTY_STRING();
		}
		else {
			RETVAL_FALSE;
		}
	}
	else {
		RETVAL_STRING( img_format, 1 );
	}

	MW_FREE_MAGICK_MEM( char *, img_format );
}
/* }}} */

/* {{{ proto float MagickGetImageGamma( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagegamma )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageGamma );
}
/* }}} */

/* {{{ proto array MagickGetImageGreenPrimary( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagegreenprimary )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageGreenPrimary( mgck_wnd, &x, &y ), x, y );
}
/* }}} */

/* {{{ proto float MagickGetImageHeight( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageheight )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageHeight );
}
/* }}} */

/* {{{ proto array MagickGetImageHistogram( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagehistogram )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	unsigned long num_colors;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHECK_ERR_RET_RESOURCE_ARR( PixelWand, MagickGetImageHistogram( mgck_wnd, &num_colors ), num_colors, mgck_wnd );
}
/* }}} */

/* {{{ proto int MagickGetImageIndex( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageindex )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageIndex );
}
/* }}} */

/* {{{ proto int MagickGetImageInterlaceScheme( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageinterlacescheme )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageInterlaceScheme );
}
/* }}} */

/* {{{ proto float MagickGetImageIterations( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageiterations )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageIterations );
}
/* }}} */

/* {{{ proto PixelWand MagickGetImageMatteColor( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagemattecolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_DO_BOOL_FUNC_RET_PIXELWAND( MagickGetImageMatteColor );
}
/* }}} */

/* {{{ proto string MagickGetImageMimeType( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagemimetype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MIME_TYPE( MagickGetImageFormat );
}
/* }}} */

/* {{{ proto string MagickGetMimeType( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetmimetype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MIME_TYPE( MagickGetFormat );
}
/* }}} */

/* {{{ proto array MagickGetImagePixels( MagickWand mgck_wnd, int x_offset, int y_offset, float columns, float rows, string map, int storage_type )
*/
ZEND_FUNCTION( magickgetimagepixels )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long x_offset, y_offset, php_storage;
	double columns, rows;
	char *map, *c;
	int map_len, pxl_arr_len, i;
	StorageType storage;

	MW_GET_8_ARG( "rllddsl",
				  &mgck_wnd_rsrc_zvl_p,
				  &x_offset, &y_offset,
				  &columns, &rows,
				  &map, &map_len,
				  &php_storage );

	MW_CHECK_PARAM_STR_LEN( map_len );

	for ( i = 0; i < map_len; i++ ) {
		c = &(map[i]);

		if ( !( *c == 'A' || *c == 'a' ||
				*c == 'B' || *c == 'b' ||
				*c == 'C' || *c == 'c' ||
				*c == 'G' || *c == 'g' ||
				*c == 'I' || *c == 'i' ||
				*c == 'K' || *c == 'k' ||
				*c == 'M' || *c == 'm' ||
				*c == 'O' || *c == 'o' ||
				*c == 'P' || *c == 'p' ||
				*c == 'R' || *c == 'r' ||
				*c == 'Y' || *c == 'y' ) ) {
			MW_SPIT_FATAL_ERR( "map parameter string can only contain the letters A B C G I K M O P R Y; see docs for details" );
			return;
		}
	}

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	pxl_arr_len = (int) ( columns * rows * map_len );

	storage = (StorageType) php_storage;

#define PRV_RET_PIXEL_ARRAY( type, return_type )  \
{   type *pxl_arr;  \
	MW_ARR_ECALLOC( type, pxl_arr, pxl_arr_len );  \
	if ( MagickGetImagePixels( mgck_wnd,  \
							   x_offset, y_offset,  \
							   (unsigned long) columns, (unsigned long) rows,  \
							   map,  \
							   storage,  \
							   (unsigned char *) pxl_arr ) == MagickFalse )  \
	{  \
		RETVAL_FALSE;  \
	}  \
	else {  \
		array_init( return_value );  \
		for ( i = 0; i < pxl_arr_len; i++ ) {  \
			MW_SET_1_RET_ARR_VAL( add_next_index_##return_type ( return_value, (return_type) (pxl_arr[i]) ) );  \
		}  \
	}  \
	efree( pxl_arr );  \
}

	switch ( storage ) {
		 case CharPixel		: PRV_RET_PIXEL_ARRAY( unsigned char,  long   );  break;
		 case ShortPixel	: PRV_RET_PIXEL_ARRAY( unsigned short, long   );  break;
		 case IntegerPixel	: PRV_RET_PIXEL_ARRAY( unsigned int,   double );  break;
		 case LongPixel		: PRV_RET_PIXEL_ARRAY( unsigned long,  double );  break;
		 case FloatPixel	: PRV_RET_PIXEL_ARRAY( float,          double );  break;
		 case DoublePixel	: PRV_RET_PIXEL_ARRAY( double,         double );  break;

		 default : MW_SPIT_FATAL_ERR( "Invalid StorageType argument supplied to function" );
				   return;
	}
}
/* }}} */

/* {{{ proto string MagickGetImageProfile( MagickWand mgck_wnd, string name )
*/
ZEND_FUNCTION( magickgetimageprofile )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *name;
	int name_len;
	unsigned long profile_len = 0;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &name, &name_len );

	MW_CHECK_PARAM_STR_LEN( name_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_FUNC_RETVAL_STRING_L( mgck_wnd, MagickGetImageProfile( mgck_wnd, name, &profile_len ), profile_len );
}
/* }}} */

/* {{{ proto array MagickGetImageRedPrimary( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageredprimary )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageRedPrimary( mgck_wnd, &x, &y ), x, y );
}
/* }}} */

/* {{{ proto int MagickGetImageRenderingIntent( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagerenderingintent )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageRenderingIntent );
}
/* }}} */

/* {{{ proto array MagickGetImageResolution( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageresolution )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageResolution( mgck_wnd, &x, &y ), x, y );
}
/* }}} */

/* {{{ proto float MagickGetImageScene( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagescene )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageScene );
}
/* }}} */

/* {{{ proto string MagickGetImageSignature( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagesignature )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( MagickWand, MagickGetImageSignature );
}
/* }}} */

/* {{{ proto int MagickGetImageSize( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagesize )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageSize );
}
/* }}} */

/* {{{ proto int MagickGetImageType( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagetype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageType );
}
/* }}} */

/* {{{ proto int MagickGetImageUnits( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimageunits )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageUnits );
}
/* }}} */

/* {{{ proto int MagickGetImageVirtualPixelMethod( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagevirtualpixelmethod )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetImageVirtualPixelMethod );
}
/* }}} */

/* {{{ proto array MagickGetImageWhitePoint( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagewhitepoint )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetImageWhitePoint( mgck_wnd, &x, &y ), x, y );
}
/* }}} */

/* {{{ proto float MagickGetImageWidth( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetimagewidth )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetImageWidth );
}
/* }}} */

/* {{{ proto int MagickGetInterlaceScheme( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetinterlacescheme )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_LONG( MagickWand, MagickGetInterlaceScheme );
}
/* }}} */

/* {{{ proto float MagickGetNumberImages( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetnumberimages )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( MagickWand, MagickGetNumberImages );
}
/* }}} */

/* {{{ proto string MagickGetPackageName( void )
*/
ZEND_FUNCTION( magickgetpackagename )
{
	MW_PRINT_DEBUG_INFO

	char *pckg_name;

/* No freeing of pkg_name after this call, because string returned is a constant string, ie "..."
*/
	pckg_name = (char *) MagickGetPackageName();

	RETURN_STRING( pckg_name, 1 );
}
/* }}} */

/* {{{ proto float MagickGetQuantumDepth( void )
*/
ZEND_FUNCTION( magickgetquantumdepth )
{
	MW_PRINT_DEBUG_INFO

	unsigned long depth_num;

	/* can cast the return string as void, because it is a string constant,
		so I do not have to worry about deallocating the memory associated
		with it -- it is done automatically
	*/
	(void *) MagickGetQuantumDepth( &depth_num );

	RETURN_LONG( (long) depth_num );
}
/* }}} */

/* {{{ proto string MagickGetReleaseDate( void )
*/
ZEND_FUNCTION( magickgetreleasedate )
{
	MW_PRINT_DEBUG_INFO

	char *rel_date;

/* No freeing of rel_date after this call, because string returned is a constant string, ie "..."
*/
	rel_date = (char *) MagickGetReleaseDate();
	
	RETURN_STRING( rel_date, 1 );
}
/* }}} */

/* {{{ proto float MagickGetResourceLimit( int resource_type )
*/
ZEND_FUNCTION( magickgetresourcelimit )
{
	MW_PRINT_DEBUG_INFO

	long rsrc_type;

	MW_GET_1_ARG( "l", &rsrc_type );

	MW_CHECK_CONSTANT( ResourceType, rsrc_type );

	RETURN_DOUBLE( (double) MagickGetResourceLimit( (ResourceType) rsrc_type ) );
}
/* }}} */

/* {{{ proto array MagickGetSamplingFactors( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetsamplingfactors )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE_ARR( MagickWand, MagickGetSamplingFactors );
}
/* }}} */

/* {{{ proto bool MagickGetWandSize( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickgetwandsize )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	unsigned long columns, rows;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_CHK_BOOL_RET_2_IDX_DBL_ARR( MagickGetSize( mgck_wnd, &columns, &rows ), (double) columns, (double) rows );
}
/* }}} */

/* {{{ proto array MagickGetVersion( void )
*/
ZEND_FUNCTION( magickgetversion )
{
	MW_PRINT_DEBUG_INFO

	char *version_str;
	unsigned long version_num;

/*  CANNOT MagickRelinquishMemory() the string returned from MagickGetVersion(), since the
	char * returned is actually a string constant, which will be automatically dealt with
	-- actually causes MEGA error if you try to clean it up manually
*/
	version_str = (char *) MagickGetVersion( &version_num );

	array_init( return_value );

	MW_SET_2_RET_ARR_VALS( add_next_index_string( return_value, version_str, 1 ),
						   add_next_index_long( return_value, (long) version_num ) );
}
/* }}} */

/* {{{ proto bool MagickGetVersionNumber( void )
*/
ZEND_FUNCTION( magickgetversionnumber )
{
	MW_PRINT_DEBUG_INFO

	unsigned long version_num;

	/*  see comment in magickgetversion() above */

	/* can cast the return string as void, because it is a string constant,
		so I do not have to worry about deallocating the memory associated
		with it -- it is done automatically
	*/
	(void *) MagickGetVersion( &version_num );

	RETURN_LONG( (long) version_num );
}
/* }}} */

/* {{{ proto bool MagickGetVersionString( void )
*/
ZEND_FUNCTION( magickgetversionstring )
{
	MW_PRINT_DEBUG_INFO

	char *version_str;

	/*  see comment in magickgetversion() above */

	version_str = (char *) MagickGetVersion( (long *) NULL );

	RETURN_STRING( version_str, 1 );
}
/* }}} */

/* {{{ proto bool MagickHasNextImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickhasnextimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickHasNextImage );
}
/* }}} */

/* {{{ proto bool MagickHasPreviousImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickhaspreviousimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickHasPreviousImage );
}
/* }}} */

/* {{{ proto bool MagickImplodeImage( MagickWand mgck_wnd, float amount )
*/
ZEND_FUNCTION( magickimplodeimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickImplodeImage );
}
/* }}} */

/* {{{ proto bool MagickLabelImage( MagickWand mgck_wnd, string label )
*/
ZEND_FUNCTION( magicklabelimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( MagickWand, MagickLabelImage );
}
/* }}} */

/* {{{ proto bool MagickLevelImage( MagickWand mgck_wnd, double black_point, float gamma, float white_point [, int channel_type] )
*/
ZEND_FUNCTION( magicklevelimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double black_point, gamma, white_point;
	long channel = -1;

	MW_GET_5_ARG( "rddd|l", &mgck_wnd_rsrc_zvl_p, &black_point, &gamma, &white_point, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickLevelImage( mgck_wnd, black_point, gamma, white_point ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickLevelImageChannel( mgck_wnd, (ChannelType) channel, black_point, gamma, white_point ) );
	}
}
/* }}} */

/* {{{ proto bool MagickMagnifyImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickmagnifyimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickMagnifyImage );
}
/* }}} */

/* {{{ proto bool MagickMapImage( MagickWand mgck_wnd, MagickWand map_wand, bool dither )
*/
ZEND_FUNCTION( magickmapimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *map_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *map_wnd_rsrc_zvl_p;
	zend_bool dither = FALSE;

	MW_GET_3_ARG( "rr|b", &mgck_wnd_rsrc_zvl_p, &map_wnd_rsrc_zvl_p, &dither );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, map_wnd, &map_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickMapImage( mgck_wnd, map_wnd, MW_MK_MGCK_BOOL(dither) ) );
}
/* }}} */

/* {{{ proto bool MagickMatteFloodfillImage( MagickWand mgck_wnd, Quantum opacity, float fuzz, mixed bordercolor_pxl_wnd, int x, int y )
*/
ZEND_FUNCTION( magickmattefloodfillimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *bordercolor_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double opacity, fuzz;
	long x, y;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 6),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a Quantum opacity value, a fuzz value, "  \
							"a bordercolor PixelWand resource (or ImageMagick color string), "  \
							"and the x and y ordinates of the starting pixel" );

	convert_to_double_ex( zvl_pp_args_arr[1] );
	opacity =  Z_DVAL_PP( zvl_pp_args_arr[1] );

	if ( opacity < 0 || opacity > MW_MaxRGB ) {
		zend_error( MW_E_ERROR, "%s(): the value of the opacity Quantum color argument was invalid. "  \
								"Quantum color values must match \"0 <= color_val <= %0.0f\"",
								get_active_function_name( TSRMLS_C ), MW_MaxRGB );
		return;
	}

	convert_to_double_ex( zvl_pp_args_arr[2] );
	fuzz =     Z_DVAL_PP( zvl_pp_args_arr[2] );

	convert_to_long_ex( zvl_pp_args_arr[4] );
	x =      Z_LVAL_PP( zvl_pp_args_arr[4] );

	convert_to_long_ex( zvl_pp_args_arr[5] );
	y =      Z_LVAL_PP( zvl_pp_args_arr[5] );

	if ( Z_TYPE_PP( zvl_pp_args_arr[3] ) == IS_RESOURCE ) {

		if (  (   MW_FETCH_RSRC( PixelWand, bordercolor_pxl_wnd, zvl_pp_args_arr[3] ) == MagickFalse
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, bordercolor_pxl_wnd, zvl_pp_args_arr[3] ) == MagickFalse )
			|| IsPixelWand( bordercolor_pxl_wnd ) == MagickFalse )
		{
			MW_SPIT_FATAL_ERR( "invalid resource type as argument #4; a PixelWand resource is required" );
			efree( zvl_pp_args_arr );
			return;
		}
		is_script_pxl_wnd = 1;
	}
	else {
		if ( Z_TYPE_PP( zvl_pp_args_arr[3] ) == IS_NULL ) {
			is_script_pxl_wnd = 1;
			bordercolor_pxl_wnd = (PixelWand *) NULL;
		}
		else {
			is_script_pxl_wnd = 0;

			bordercolor_pxl_wnd = (PixelWand *) NewPixelWand();

			if ( bordercolor_pxl_wnd == (PixelWand *) NULL ) {
				MW_SPIT_FATAL_ERR( "unable to create necessary PixelWand" );
				efree( zvl_pp_args_arr );
				return;
			}
			convert_to_string_ex( zvl_pp_args_arr[3] );

			if ( Z_STRLEN_PP( zvl_pp_args_arr[3] ) > 0 ) {

				if ( PixelSetColor( bordercolor_pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[3] ) ) == MagickFalse ) {

					MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	bordercolor_pxl_wnd, PixelGetException,
														"could not set PixelWand to desired fill color"
					);

					bordercolor_pxl_wnd = DestroyPixelWand( bordercolor_pxl_wnd );
					efree( zvl_pp_args_arr );

					return;
				}
			}
		}
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickMatteFloodfillImage( mgck_wnd, (Quantum) opacity, fuzz, bordercolor_pxl_wnd, x, y ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		bordercolor_pxl_wnd = DestroyPixelWand( bordercolor_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickMedianFilterImage( MagickWand mgck_wnd, float radius )
*/
ZEND_FUNCTION( magickmedianfilterimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickMedianFilterImage );
}
/* }}} */

/* {{{ proto bool MagickMinifyImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickminifyimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickMinifyImage );
}
/* }}} */

/* {{{ proto bool MagickModulateImage( MagickWand mgck_wnd, float brightness, float saturation, float hue )
*/
ZEND_FUNCTION( magickmodulateimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double brightness, saturation, hue;

	MW_GET_4_ARG( "rddd", &mgck_wnd_rsrc_zvl_p, &brightness, &saturation, &hue );

	MW_CHECK_VALUE_RANGE( brightness, -100.0, 100.0 );
	MW_CHECK_VALUE_RANGE( saturation, -100.0, 100.0 );
	MW_CHECK_VALUE_RANGE( hue,        -100.0, 100.0 );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickModulateImage( mgck_wnd, brightness, saturation, hue ) );
}
/* }}} */

/* {{{ proto MagickWand MagickMontageImage( MagickWand mgck_wnd, DrawingWand drw_wnd, string tile_geometry, string thumbnail_geometry, int montage_mode, string frame )
*/
ZEND_FUNCTION( magickmontageimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	DrawingWand *drw_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *drw_wnd_rsrc_zvl_p;
	char *tile_geom, *thumb_geom, *frame;
	int tile_geom_len, thumb_geom_len, frame_len;
	long mode;

	MW_GET_9_ARG( "rrssls",
				  &mgck_wnd_rsrc_zvl_p, &drw_wnd_rsrc_zvl_p,
				  &tile_geom, &tile_geom_len,
				  &thumb_geom, &thumb_geom_len,
				  &mode,
				  &frame, &frame_len );

	MW_CHECK_PARAM_STR_LEN_EX( (tile_geom_len == 0 || thumb_geom_len == 0 || frame_len == 0) );

	MW_CHECK_CONSTANT( MontageMode, mode );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( DrawingWand, drw_wnd, &drw_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickMontageImage( mgck_wnd, drw_wnd, tile_geom, thumb_geom, (MontageMode) mode, frame ) );
}
/* }}} */

/* {{{ proto MagickWand MagickMorphImages( MagickWand mgck_wnd, float number_frames )
*/
ZEND_FUNCTION( magickmorphimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double num_frames;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &num_frames );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickMorphImages( mgck_wnd, (unsigned long) num_frames ) );
}
/* }}} */

/* {{{ proto MagickWand MagickMosaicImages( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickmosaicimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickMosaicImages( mgck_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickMotionBlurImage( MagickWand mgck_wnd, float radius, float sigma, float angle )
*/
ZEND_FUNCTION( magickmotionblurimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma, angle;

	MW_GET_4_ARG( "rddd", &mgck_wnd_rsrc_zvl_p, &radius, &sigma, &angle );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickMotionBlurImage( mgck_wnd, radius, sigma, angle ) );
}
/* }}} */

/* {{{ proto bool MagickNegateImage( MagickWand mgck_wnd [, bool only_the_gray [, int channel_type]] )
*/
ZEND_FUNCTION( magicknegateimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	zend_bool only_the_gray = FALSE;
	long channel = -1;

	MW_GET_3_ARG( "r|bl", &mgck_wnd_rsrc_zvl_p, &only_the_gray, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickNegateImage( mgck_wnd, MW_MK_MGCK_BOOL(only_the_gray) ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickNegateImageChannel( mgck_wnd, (ChannelType) channel, MW_MK_MGCK_BOOL(only_the_gray) ) );
	}
}
/* }}} */

/* {{{ proto bool MagickNewImage( MagickWand mgck_wnd, float width, float height [, string imagemagick_col_str] )
*/
ZEND_FUNCTION( magicknewimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *bg_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double width, height;
	long cur_idx;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count < 3 || arg_count > 4),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a desired image width and height, and an optional " \
							"background color PixelWand resource or ImageMagick color string" );

	convert_to_double_ex( zvl_pp_args_arr[1] );
	convert_to_double_ex( zvl_pp_args_arr[2] );

	width  = Z_DVAL_PP( zvl_pp_args_arr[1] );
	height = Z_DVAL_PP( zvl_pp_args_arr[2] );

	if ( width < 1 || height < 1 ) {
		MW_SPIT_FATAL_ERR( "cannot create an image smaller than 1 x 1 pixels in area" );
		efree( zvl_pp_args_arr );
		return;
	}

	if ( arg_count == 4 && Z_TYPE_PP( zvl_pp_args_arr[3] ) == IS_RESOURCE ) {

		if (  (   MW_FETCH_RSRC( PixelWand, bg_pxl_wnd, zvl_pp_args_arr[3] ) == MagickFalse
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, bg_pxl_wnd, zvl_pp_args_arr[3] ) == MagickFalse )
			|| IsPixelWand( bg_pxl_wnd ) == MagickFalse )
		{
			MW_SPIT_FATAL_ERR( "invalid resource type as fourth argument; a PixelWand resource is required" );
			efree( zvl_pp_args_arr );
			return;
		}
		is_script_pxl_wnd = 1;
	}
	else {
		is_script_pxl_wnd = 0;

		bg_pxl_wnd = (PixelWand *) NewPixelWand();

		if ( bg_pxl_wnd == (PixelWand *) NULL ) {
			MW_SPIT_FATAL_ERR( "unable to create necessary background color PixelWand" );
			efree( zvl_pp_args_arr );
			return;
		}
		if ( arg_count == 4 ) {
			convert_to_string_ex( zvl_pp_args_arr[3] );

			if ( Z_STRLEN_PP( zvl_pp_args_arr[3] ) > 0 ) {

				if ( PixelSetColor( bg_pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[3] ) ) == MagickFalse ) {

					MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	bg_pxl_wnd, PixelGetException,
														"could not set PixelWand to desired fill color"
					);

					bg_pxl_wnd = DestroyPixelWand( bg_pxl_wnd );
					efree( zvl_pp_args_arr );

					return;
				}
			}
		}
	}

	cur_idx = (long) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException ) {
		cur_idx = -1;
	}
	MagickClearException( mgck_wnd );

	if (   MagickNewImage( mgck_wnd, (unsigned long) width, (unsigned long) height, bg_pxl_wnd ) == MagickTrue
		&& MagickSetImageIndex( mgck_wnd, (cur_idx + 1) ) == MagickTrue )
	{
		RETVAL_TRUE;
	}
	else {
		MW_API_FUNC_FAIL_CHECK_WAND_ERROR( mgck_wnd, MagickGetException, "unable to create new image of desired color" );
	}

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		bg_pxl_wnd = DestroyPixelWand( bg_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickNextImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magicknextimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickNextImage );
}
/* }}} */

/* {{{ proto bool MagickNormalizeImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magicknormalizeimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickNormalizeImage );
}
/* }}} */

/* {{{ proto bool MagickOilPaintImage( MagickWand mgck_wnd, float radius )
*/
ZEND_FUNCTION( magickoilpaintimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickOilPaintImage );
}
/* }}} */

/* {{{ proto bool MagickPaintOpaqueImage( MagickWand mgck_wnd, mixed target_pxl_wnd, mixed fill_pxl_wnd [, float fuzz] )
*/
ZEND_FUNCTION( magickpaintopaqueimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *target_pxl_wnd, *fill_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd, is_script_pxl_wnd_2;
	double fuzz = 0.0;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count < 3 || arg_count > 4),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a target color PixelWand resource (or ImageMagick color "  \
							"string), a fill color PixelWand resource (or ImageMagick color "  \
							"string), and an optional fuzz value" );

	if ( arg_count == 4 ) {
		convert_to_double_ex( zvl_pp_args_arr[3] );
		fuzz =     Z_DVAL_PP( zvl_pp_args_arr[3] );
	}

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, target_pxl_wnd, is_script_pxl_wnd );

	if ( Z_TYPE_PP( zvl_pp_args_arr[2] ) == IS_RESOURCE ) {

		if (  (   MW_FETCH_RSRC( PixelWand, fill_pxl_wnd, zvl_pp_args_arr[2] ) == MagickFalse
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, fill_pxl_wnd, zvl_pp_args_arr[2] ) == MagickFalse )
			|| IsPixelWand( fill_pxl_wnd ) == MagickFalse )
		{
			MW_SPIT_FATAL_ERR( "invalid resource type as argument #3; a PixelWand resource is required" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				target_pxl_wnd = DestroyPixelWand( target_pxl_wnd );
			}
			return;
		}
		is_script_pxl_wnd_2 = 1;
	}
	else {
		is_script_pxl_wnd_2 = 0;

		fill_pxl_wnd = (PixelWand *) NewPixelWand();

		if ( fill_pxl_wnd == (PixelWand *) NULL ) {
			MW_SPIT_FATAL_ERR( "unable to create necessary PixelWand" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				target_pxl_wnd = DestroyPixelWand( target_pxl_wnd );
			}
			return;
		}
		convert_to_string_ex( zvl_pp_args_arr[2] );

		if ( Z_STRLEN_PP( zvl_pp_args_arr[2] ) > 0 ) {

			if ( PixelSetColor( fill_pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[2] ) ) == MagickFalse ) {

				MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	fill_pxl_wnd, PixelGetException,
													"could not set PixelWand to desired fill color"
				);

				fill_pxl_wnd = DestroyPixelWand( fill_pxl_wnd );
				efree( zvl_pp_args_arr );
				if ( !is_script_pxl_wnd ) {
					target_pxl_wnd = DestroyPixelWand( target_pxl_wnd );
				}

				return;
			}
		}
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickPaintOpaqueImage( mgck_wnd, target_pxl_wnd, fill_pxl_wnd, fuzz ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		target_pxl_wnd = DestroyPixelWand( target_pxl_wnd );
	}
	if ( !is_script_pxl_wnd_2 ) {
		fill_pxl_wnd = DestroyPixelWand( fill_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickPaintTransparentImage( MagickWand mgck_wnd, mixed target_pxl_wnd [, float opacity [, float fuzz]] )
*/
ZEND_FUNCTION( magickpainttransparentimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *target_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double opacity = (double) TransparentOpacity, fuzz = 0.0;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count < 2 || arg_count > 4),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a target color PixelWand resource (or ImageMagick color "  \
							"string), a fill color PixelWand resource (or ImageMagick color "  \
							"string), and an optional fuzz value" );

	if ( arg_count > 2 ) {
		convert_to_double_ex( zvl_pp_args_arr[2] );
		opacity =  Z_DVAL_PP( zvl_pp_args_arr[2] );

		if ( arg_count == 4 ) {
			convert_to_double_ex( zvl_pp_args_arr[3] );
			fuzz =     Z_DVAL_PP( zvl_pp_args_arr[3] );
		}
	}

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, target_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickPaintTransparentImage( mgck_wnd, target_pxl_wnd, (Quantum) opacity, fuzz ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		target_pxl_wnd = DestroyPixelWand( target_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickPingImage( MagickWand mgck_wnd, string filename )
*/
ZEND_FUNCTION( magickpingimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *filename;
	int filename_len;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &filename, &filename_len );

	MW_CHECK_PARAM_STR_LEN( filename_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_FILE_IS_READABLE( filename );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickPingImage( mgck_wnd, filename ) );
}
/* }}} */

/* {{{ proto bool MagickPosterizeImage( MagickWand mgck_wnd, float levels, bool dither )
*/
ZEND_FUNCTION( magickposterizeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double levels;
	zend_bool dither = FALSE;

	MW_GET_3_ARG( "rd|b", &mgck_wnd_rsrc_zvl_p, &levels, &dither );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickPosterizeImage( mgck_wnd, (unsigned long) levels, MW_MK_MGCK_BOOL(dither) ) );
}
/* }}} */

/* {{{ proto MagickWand MagickPreviewImages( MagickWand mgck_wnd, PreviewType preview )
*/
ZEND_FUNCTION( magickpreviewimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long preview;

	MW_GET_2_ARG( "rl", &mgck_wnd_rsrc_zvl_p, &preview );

	MW_CHECK_CONSTANT( PreviewType, preview );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickPreviewImages( mgck_wnd, (PreviewType) preview ) );
}
/* }}} */

/* {{{ proto bool MagickPreviousImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickpreviousimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickPreviousImage );
}
/* }}} */

/* {{{ proto bool MagickProfileImage( MagickWand mgck_wnd, string name [, string profile] )
*/
ZEND_FUNCTION( magickprofileimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *name, *profile = (char *) NULL;
	int name_len, profile_len = 0;

	MW_GET_5_ARG( "rs|s", &mgck_wnd_rsrc_zvl_p, &name, &name_len, &profile, &profile_len );

	MW_CHECK_PARAM_STR_LEN( name_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickProfileImage( mgck_wnd,
												  name,
												  (unsigned char *) (profile_len == 0 ? NULL : profile),
												  (unsigned long) profile_len
							  )
	);
}
/* }}} */

/* {{{ proto bool MagickQuantizeImage( MagickWand mgck_wnd, float number_colors, int colorspace_type, float treedepth, bool dither, bool measure_error )
*/
ZEND_FUNCTION( magickquantizeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double num_cols, treedepth;
	long col_space;
	zend_bool dither, measure_err;

	MW_GET_6_ARG( "rdldbb", &mgck_wnd_rsrc_zvl_p, &num_cols, &col_space, &treedepth, &dither, &measure_err );

	MW_CHECK_CONSTANT( ColorspaceType, col_space );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL(
		MagickQuantizeImage(
			mgck_wnd,
			(unsigned long) num_cols,
			(ColorspaceType) col_space,
			(unsigned long) treedepth,
			MW_MK_MGCK_BOOL(dither),
			MW_MK_MGCK_BOOL(measure_err)
		)
	);
}
/* }}} */

/* {{{ proto bool MagickQuantizeImages( MagickWand mgck_wnd, float number_colors, int colorspace_type, float treedepth, bool dither, bool measure_error )
*/
ZEND_FUNCTION( magickquantizeimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double num_cols, treedepth;
	long col_space;
	zend_bool dither, measure_err;

	MW_GET_6_ARG( "rdldbb", &mgck_wnd_rsrc_zvl_p, &num_cols, &col_space, &treedepth, &dither, &measure_err );

	MW_CHECK_CONSTANT( ColorspaceType, col_space );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL(
		MagickQuantizeImages(
			mgck_wnd,
			(unsigned long) num_cols,
			(ColorspaceType) col_space,
			(unsigned long) treedepth,
			MW_MK_MGCK_BOOL(dither),
			MW_MK_MGCK_BOOL(measure_err)
		)
	);
}
/* }}} */

/* {{{ proto string MagickQueryConfigureOption( string option )
*/
ZEND_FUNCTION( magickqueryconfigureoption )
{
	MW_PRINT_DEBUG_INFO

	char *option, *value;
	int option_len;

	MW_GET_2_ARG( "s", &option, &option_len );

	MW_CHECK_PARAM_STR_LEN( option_len );

	value = (char *) MagickQueryConfigureOption( option );

	if ( value == (char *) NULL || *value == '\0' ) {
		RETURN_MW_EMPTY_STRING();
	}

	RETVAL_STRING( value, 1 );
	MW_FREE_MAGICK_MEM( char *, value );
}
/* }}} */

/* {{{ proto array MagickQueryConfigureOptions( string pattern )
*/
ZEND_FUNCTION( magickqueryconfigureoptions )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_QUERY_STRING_ARR( MagickQueryConfigureOptions );
}
/* }}} */

/* {{{ proto array MagickQueryFontMetrics( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickqueryfontmetrics )
{
	MW_PRINT_DEBUG_INFO

	MW_INIT_FONT_METRIC_ARRAY( font_metric_arr );

	if ( font_metric_arr == (double *) NULL ) {
		RETURN_FALSE;
	}
	else {
		int i;

		array_init( return_value );

		for ( i = 0; i < 7; i++ ) {
			if ( add_next_index_double( return_value, font_metric_arr[i] ) == FAILURE ) {
				MW_SPIT_FATAL_ERR( "error adding a value to the return array" );
				break;
			}
		}
		font_metric_arr = MagickRelinquishMemory( font_metric_arr );
	}
}
/* }}} */

/* {{{ proto float MagickGetCharWidth( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgetcharwidth )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 0 );
}
/* }}} */

/* {{{ proto float MagickGetCharHeight( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgetcharheight )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 1 );
}
/* }}} */

/* {{{ proto float MagickGetTextAscent( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgettextascent )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 2 );
}
/* }}} */

/* {{{ proto float MagickGetTextDescent( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgettextdescent )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 3 );
}
/* }}} */

/* {{{ proto float MagickGetStringWidth( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgetstringwidth )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 4 );
}
/* }}} */

/* {{{ proto float MagickGetStringHeight( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgetstringheight )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 5 );
}
/* }}} */

/* {{{ proto float MagickGetMaxTextAdvance( MagickWand mgck_wnd, DrawingWand drw_wnd, string txt [, bool multiline] )
*/
ZEND_FUNCTION( magickgetmaxtextadvance )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_FONT_METRIC( 6 );
}
/* }}} */

/* {{{ proto array MagickQueryFonts( string pattern )
*/
ZEND_FUNCTION( magickqueryfonts )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_QUERY_STRING_ARR( MagickQueryFonts );
}
/* }}} */

/* {{{ proto array MagickQueryFormats( string pattern )
*/
ZEND_FUNCTION( magickqueryformats )
{
	MW_PRINT_DEBUG_INFO

	MW_RETURN_QUERY_STRING_ARR( MagickQueryFormats );
}
/* }}} */

/* {{{ proto bool MagickRadialBlurImage( MagickWand mgck_wnd, float angle )
*/
ZEND_FUNCTION( magickradialblurimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickRadialBlurImage );
}
/* }}} */

/* {{{ proto bool MagickRaiseImage( MagickWand mgck_wnd, float width, float height, int x, int y, bool raise )
*/
ZEND_FUNCTION( magickraiseimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double width, height, x, y;
	zend_bool do_raise = TRUE;

	MW_GET_6_ARG( "rdddd|b", &mgck_wnd_rsrc_zvl_p, &width, &height, &x, &y, &do_raise );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickRaiseImage( mgck_wnd,
												(unsigned long) width, (unsigned long) height,
												(unsigned long) x,     (unsigned long) y,
												MW_MK_MGCK_BOOL(do_raise) ) );
}
/* }}} */

/* {{{ proto bool MagickReadImage( MagickWand mgck_wnd, string filename )
*/
ZEND_FUNCTION( magickreadimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *filename, *real_filename = (char *) NULL, is_magick_format;
	int filename_len;
	php_stream *stream;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &filename, &filename_len );

	MW_CHECK_PARAM_STR_LEN( filename_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	is_magick_format = MW_read_magick_format( mgck_wnd, filename );

	if ( is_magick_format == 1 ) {
		RETURN_TRUE;
	}
	else {
		if ( is_magick_format == -1 ) {
			return;
		}
	}

	/* Finally, open a file for reading, obeying all currently implemented php.ini restrictions */
	stream = php_stream_open_wrapper( filename, "rb", MW_STD_STREAM_OPTS, &real_filename );

	if ( stream == (php_stream *) NULL ) {
		/* PHP cannot read image to the current filename: output fatal error, with reason */
		zend_error( MW_E_ERROR, "%s(): PHP cannot read %s; possible php.ini restrictions",
								get_active_function_name(TSRMLS_C), filename );
	}
	else {
		if ( MW_read_image_from_php_stream( mgck_wnd, stream, real_filename ) == MagickTrue ) {
			RETVAL_TRUE;
		}
		php_stream_close( stream );
	}

	MW_EFREE_MEM( char *, real_filename );
}
/* }}} */

/* {{{ proto bool MagickReadImageBlob( MagickWand mgck_wnd, string blob )
*/
ZEND_FUNCTION( magickreadimageblob )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *blob;
	int blob_len;
	unsigned long orig_img_idx;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &blob, &blob_len );

	MW_CHECK_PARAM_STR_LEN( blob_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	orig_img_idx = (unsigned long) MagickGetNumberImages( mgck_wnd );

	if( MagickReadImageBlob( mgck_wnd, (unsigned char *) blob, (size_t) blob_len ) == MagickTrue ) {

		if ( MagickSetImageIndex( mgck_wnd, (long) orig_img_idx ) == MagickTrue ) {
			while ( MagickSetImageFilename( mgck_wnd, "" ), MagickNextImage( mgck_wnd ) == MagickTrue )
				;
		}
		MagickClearException( mgck_wnd );
		MagickResetIterator( mgck_wnd );
		RETVAL_TRUE;
	}
	else {
		/* C API cannot read the image(s) from the BLOB: output error, with reason */
		MW_API_FUNC_FAIL_CHECK_WAND_ERROR( mgck_wnd, MagickGetException, "unable to read the supplied BLOB argument" );
	}
}
/* }}} */

/* {{{ proto bool MagickReadImageFile( MagickWand mgck_wnd, php_stream handle )
*/
ZEND_FUNCTION( magickreadimagefile )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_stream;
	php_stream *stream;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &zvl_stream );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	php_stream_from_zval( stream, &zvl_stream );

	/* You can now use the stream.  However, you do not "own" the stream, the script does.
	   That means you MUST NOT close the stream, because it will cause PHP to crash!
	*/

/*  php_stream_seek( stream, 0, SEEK_SET ); */

	if ( MW_read_image_from_php_stream( mgck_wnd, stream, "the filehandle argument" ) == MagickTrue ) {
		RETVAL_TRUE;
	}
}
/* }}} */

/* {{{ proto bool MagickReadImages( MagickWand mgck_wand, array img_filenames_array )
*/
ZEND_FUNCTION( magickreadimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_arr, **zvl_pp_element;
	int num_files, i;
	HashPosition pos;
	php_stream *stream;
	char *filename, *real_filename = (char *) NULL, is_magick_format;

	MW_GET_2_ARG( "ra", &mgck_wnd_rsrc_zvl_p, &zvl_arr );

	num_files = zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

	if ( num_files < 1 ) {
		zend_error( MW_E_ERROR, "%s(): function requires an array containing at least 1 image filename",
								get_active_function_name(TSRMLS_C) );
		return;
	}

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	i = 0;

	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element ) {
		convert_to_string_ex( zvl_pp_element );

		if ( Z_STRLEN_PP( zvl_pp_element ) < 1 ) {
			zend_error( MW_E_ERROR, "%s(): image filename at index %d of argument array was empty",
									get_active_function_name(TSRMLS_C), i );
			return;
		}

		filename = Z_STRVAL_PP( zvl_pp_element );

		is_magick_format = MW_read_magick_format( mgck_wnd, filename );

		if ( is_magick_format == 1 ) {
			continue;
		}
		else {
			if ( is_magick_format == -1 ) {
				return;
			}
		}

		/* Finally, open a file for reading, obeying all currently implemented php.ini restrictions */
		stream = php_stream_open_wrapper( filename, "rb", MW_STD_STREAM_OPTS, &real_filename );

		if ( stream == (php_stream *) NULL ) {
			/* PHP cannot open image with the current filename: output fatal error, with reason */
			zend_error( MW_E_ERROR, "%s(): PHP cannot read %s; possible php.ini restrictions",
									get_active_function_name(TSRMLS_C), filename );
			MW_EFREE_MEM( char *, real_filename );
			return;
		}
		else {
			if ( MW_read_image_from_php_stream( mgck_wnd, stream, real_filename ) == MagickFalse ) {
				MW_EFREE_MEM( char *, real_filename );
				php_stream_close( stream );
				return;
			}
			php_stream_close( stream );
		}

		MW_EFREE_MEM( char *, real_filename );
		i++;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool MagickReduceNoiseImage( MagickWand mgck_wnd, float radius )
*/
ZEND_FUNCTION( magickreducenoiseimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickReduceNoiseImage );
}
/* }}} */

/* {{{ proto bool MagickRemoveImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickremoveimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( MagickWand, MagickRemoveImage );
}
/* }}} */

/* {{{ proto string MagickRemoveImageProfile( MagickWand mgck_wnd, string name )
*/
ZEND_FUNCTION( magickremoveimageprofile )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *name;
	int name_len;
	unsigned long profile_len = 0;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &name, &name_len );

	MW_CHECK_PARAM_STR_LEN( name_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_FUNC_RETVAL_STRING_L( mgck_wnd, MagickRemoveImageProfile( mgck_wnd, name, &profile_len ), profile_len );
/*
	unsigned char *profile;

	profile = (unsigned char *) MagickRemoveImageProfile( mgck_wnd, name, &length );
	MW_FREE_MAGICK_MEM( unsigned char *, profile  );

	if ( mgck_wnd->exception.severity == UndefinedException ) {
		RETURN_TRUE;
	}
	else {
		RETURN_FALSE;
	}
*/
}
/* }}} */

/* {{{ proto bool MagickRemoveImageProfiles( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickremoveimageprofiles )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickProfileImage( mgck_wnd, "*", (unsigned char *) NULL, 0 ) );
}
/* }}} */

/* {{{ proto void MagickResetIterator( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickresetiterator )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MagickResetIterator( mgck_wnd );
}
/* }}} */

/* {{{ proto bool MagickResampleImage( MagickWand mgck_wand, float x_resolution, float y_resolution, int filter_type, float blur )
*/
ZEND_FUNCTION( magickresampleimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x_resolution, y_resolution, blur;
	long filter;

	MW_GET_5_ARG( "rddld", &mgck_wnd_rsrc_zvl_p, &x_resolution, &y_resolution, &filter, &blur );

	MW_CHECK_CONSTANT( FilterTypes, filter );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickResampleImage( mgck_wnd, x_resolution, y_resolution, (FilterTypes) filter, blur ) );
}
/* }}} */

/* {{{ proto bool MagickResizeImage( MagickWand mgck_wand, float columns, float rows, int filter_type, float blur )
*/
ZEND_FUNCTION( magickresizeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long filter;
	double columns, rows, blur;

	MW_GET_5_ARG( "rddld", &mgck_wnd_rsrc_zvl_p, &columns, &rows, &filter, &blur );

	MW_CHECK_CONSTANT( FilterTypes, filter );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickResizeImage( mgck_wnd, (unsigned long) columns, (unsigned long) rows, (FilterTypes) filter, blur ) );
}
/* }}} */

/* {{{ proto bool MagickRollImage( MagickWand mgck_wnd, int x_offset, int y_offset )
*/
ZEND_FUNCTION( magickrollimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long x_offset, y_offset;

	MW_GET_3_ARG( "rll", &mgck_wnd_rsrc_zvl_p, &x_offset, &y_offset );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickRollImage( mgck_wnd, x_offset, y_offset ) );
}
/* }}} */

/* {{{ proto bool MagickRotateImage( MagickWand mgck_wnd, mixed background_pxl_wnd, float degrees )
*/
ZEND_FUNCTION( magickrotateimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *background_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double degrees;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 3),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a background color PixelWand resource (or "  \
							"ImageMagick color string), and the desired degrees of rotation" );

	convert_to_double_ex( zvl_pp_args_arr[2] );
	degrees =  Z_DVAL_PP( zvl_pp_args_arr[2] );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, background_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickRotateImage( mgck_wnd, background_pxl_wnd, degrees ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		background_pxl_wnd = DestroyPixelWand( background_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickSampleImage( MagickWand mgck_wnd, float columns, float rows )
*/
ZEND_FUNCTION( magicksampleimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double columns, rows;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &columns, &rows );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSampleImage( mgck_wnd, (unsigned long) columns, (unsigned long) rows ) );
}
/* }}} */

/* {{{ proto bool MagickScaleImage( MagickWand mgck_wnd, float columns, float rows )
*/
ZEND_FUNCTION( magickscaleimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double columns, rows;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &columns, &rows );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickScaleImage( mgck_wnd, (unsigned long) columns, (unsigned long) rows ) );
}
/* }}} */

/* {{{ proto bool MagickSeparateImageChannel( MagickWand mgck_wnd, int channel_type )
*/
ZEND_FUNCTION( magickseparateimagechannel )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( ChannelType, MagickSeparateImageChannel );
}
/* }}} */

/* {{{ proto bool MagickSetCompressionQuality( MagickWand mgck_wnd, float quality )
*/
ZEND_FUNCTION( magicksetcompressionquality )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double quality;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &quality );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetCompressionQuality( mgck_wnd, (unsigned long) quality ) );
}
/* }}} */

/* {{{ proto bool MagickSetFilename( MagickWand mgck_wnd [, string filename] )
*/
ZEND_FUNCTION( magicksetfilename )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *filename;
	int filename_len = 0;

	MW_GET_3_ARG( "r|s", &mgck_wnd_rsrc_zvl_p, &filename, &filename_len );

/* The following is commented out as this function can be used to REMOVE an image filename, by
   specifying NULL or "" as the filename argument

	MW_CHECK_PARAM_STR_LEN( filename_len );
*/
	if ( filename_len == 0 ) {
		*filename = '\0';
	}

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetFilename( mgck_wnd, filename ) );
}
/* }}} */

/* {{{ proto void MagickSetFirstIterator( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magicksetfirstiterator )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MagickSetFirstIterator( mgck_wnd );
}
/* }}} */

/* {{{ proto bool MagickSetFormat( MagickWand mgck_wnd, string format )
*/
ZEND_FUNCTION( magicksetformat )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *format, **formats_arr;
	int format_len;
	unsigned long num_formats = 0, i;
	MagickBooleanType is_good_format = MagickFalse;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &format, &format_len );

	if ( format_len < 2 ) {
		zend_error( MW_E_ERROR, "%s(): \"%s\" is not a valid image format", 
								get_active_function_name(TSRMLS_C), format );
		return;
	}

	formats_arr = (char **) MagickQueryFormats( format, &num_formats );

	if ( num_formats < 1 || formats_arr == (char **) NULL ) {
		zend_error( MW_E_ERROR, "%s(): \"%s\" is not a valid image format", 
								get_active_function_name(TSRMLS_C), format );
		MW_FREE_MAGICK_MEM( char **, formats_arr );
		return;
	}
	else {
		if ( num_formats > 0 && formats_arr != (char **) NULL ) {
			for ( i = 0; i < num_formats; i++ ) {
				if ( strncasecmp( formats_arr[i], format, (size_t) format_len ) == 0 ) {
					is_good_format = MagickTrue;
					break;
				}
			}
		}
	}

	MW_FREE_MAGICK_MEM( char **, formats_arr );

	if ( is_good_format == MagickTrue ) {

		MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

		if( MagickSetFormat( mgck_wnd, format ) == MagickTrue ) {
			RETURN_TRUE;
		}
		else {
			RETURN_FALSE;
		}
	}
	else {
		zend_error( MW_E_ERROR, "%s(): \"%s\" is not a valid image format", 
								get_active_function_name(TSRMLS_C), format );
		return;
	}
}
/* }}} */

/* {{{ proto bool MagickSetImage( MagickWand mgck_wnd, MagickWand replace_wand )
*/
ZEND_FUNCTION( magicksetimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *set_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *set_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &set_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );
	MW_GET_POINTER_FROM_RSRC( MagickWand, set_wnd, &set_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImage( mgck_wnd, set_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageBackgroundColor( MagickWand mgck_wnd, mixed background_pxl_wnd )
*/
ZEND_FUNCTION( magicksetimagebackgroundcolor )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *background_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource and a background color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, background_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageBackgroundColor( mgck_wnd, background_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		background_pxl_wnd = DestroyPixelWand( background_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageBias( MagickWand mgck_wnd, float bias )
*/
ZEND_FUNCTION( magicksetimagebias )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickSetImageBias );
}
/* }}} */

/* {{{ proto bool MagickSetImageBluePrimary( MagickWand mgck_wnd, float x, float y )
*/
ZEND_FUNCTION( magicksetimageblueprimary )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageBluePrimary( mgck_wnd, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageBorderColor( MagickWand mgck_wnd, mixed border_pxl_wnd )
*/
ZEND_FUNCTION( magicksetimagebordercolor )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *border_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource and a border color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, border_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageBorderColor( mgck_wnd, border_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		border_pxl_wnd = DestroyPixelWand( border_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageColormapColor( MagickWand mgck_wnd, float index, mixed mapcolor_pxl_wnd )
*/
ZEND_FUNCTION( magicksetimagecolormapcolor )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *mapcolor_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double index;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 3),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, the image colormap offset, and "  \
							"a map color PixelWand resource (or ImageMagick color string)" );

	convert_to_double_ex( zvl_pp_args_arr[1] );
	index =    Z_DVAL_PP( zvl_pp_args_arr[1] );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 2, 3, mapcolor_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageColormapColor( mgck_wnd, (unsigned long) index, mapcolor_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		mapcolor_pxl_wnd = DestroyPixelWand( mapcolor_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageColorspace( MagickWand mgck_wnd, int colorspace_type )
*/
ZEND_FUNCTION( magicksetimagecolorspace )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( ColorspaceType, MagickSetImageColorspace );
}
/* }}} */

/* {{{ proto bool MagickSetImageCompose( MagickWand mgck_wnd, int composite_operator )
*/
ZEND_FUNCTION( magicksetimagecompose )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( CompositeOperator, MagickSetImageCompose );
}
/* }}} */

/* {{{ proto bool MagickSetImageCompression( MagickWand mgck_wnd, int compression_type )
*/
ZEND_FUNCTION( magicksetimagecompression )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( CompressionType, MagickSetImageCompression );
}
/* }}} */

/* {{{ proto bool MagickSetImageCompressionQuality( MagickWand mgck_wnd, float quality )
*/
ZEND_FUNCTION( magicksetimagecompressionquality )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double quality;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &quality );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageCompressionQuality( mgck_wnd, (unsigned long) quality ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageDelay( MagickWand mgck_wnd, float delay )
*/
ZEND_FUNCTION( magicksetimagedelay )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double delay;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &delay );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageDelay( mgck_wnd, (unsigned long) delay ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageDepth( MagickWand mgck_wnd, int depth [, int channel_type]] )
*/
ZEND_FUNCTION( magicksetimagedepth )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long depth, channel = -1;

	MW_GET_3_ARG( "rl|l", &mgck_wnd_rsrc_zvl_p, &depth, &channel );

	if ( !( depth == 8 || depth == 16 || depth == 32 ) ) {
		zend_error( MW_E_ERROR, "%s(): image channel depth argument cannot be %d; it must be either 8, 16, or 32",
								get_active_function_name(TSRMLS_C), depth );
		RETURN_FALSE;
	}

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageDepth( mgck_wnd, (unsigned long) depth ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageChannelDepth( mgck_wnd, (ChannelType) channel, (unsigned long) depth ) );
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageDispose( MagickWand mgck_wnd, int dispose_type )
*/
ZEND_FUNCTION( magicksetimagedispose )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( DisposeType, MagickSetImageDispose );
}
/* }}} */

/* {{{ proto bool MagickSetImageFilename( MagickWand mgck_wnd [, string filename] )
*/
ZEND_FUNCTION( magicksetimagefilename )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *filename;
	int filename_len = 0;

	MW_GET_3_ARG( "r|s", &mgck_wnd_rsrc_zvl_p, &filename, &filename_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

/* This function can be used to REMOVE an image filename, by specifying NULL or "" as the filename argument
*/
	if ( filename_len < 1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageFilename( mgck_wnd, "" ) );
	}
	else {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageFilename( mgck_wnd, filename ) );
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageFormat( MagickWand mgck_wnd, string format )
*/
ZEND_FUNCTION( magicksetimageformat )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *format, **formats_arr;
	int format_len;
	unsigned long num_formats = 0, i;
	MagickBooleanType is_good_format = MagickFalse;

	MW_GET_3_ARG( "rs", &mgck_wnd_rsrc_zvl_p, &format, &format_len );

	if ( format_len < 2 ) {
		zend_error( MW_E_ERROR, "%s(): \"%s\" is not a valid image format", 
								get_active_function_name(TSRMLS_C), format );
		return;
	}

	formats_arr = (char **) MagickQueryFormats( format, &num_formats );

	if ( num_formats < 1 || formats_arr == (char **) NULL ) {
		zend_error( MW_E_ERROR, "%s(): \"%s\" is not a valid image format", 
								get_active_function_name(TSRMLS_C), format );
		MW_FREE_MAGICK_MEM( char **, formats_arr );
		return;
	}
	else {
		if ( num_formats > 0 && formats_arr != (char **) NULL ) {
			for ( i = 0; i < num_formats; i++ ) {
				if ( strncasecmp( formats_arr[i], format, (size_t) format_len ) == 0 ) {
					is_good_format = MagickTrue;
					break;
				}
			}
		}
	}

	MW_FREE_MAGICK_MEM( char **, formats_arr );

	if ( is_good_format == MagickTrue ) {

		MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

		if( MagickSetImageFormat( mgck_wnd, format ) == MagickTrue ) {
			RETURN_TRUE;
		}
		else {
			RETURN_FALSE;
		}
	}
	else {
		zend_error( MW_E_ERROR, "%s(): \"%s\" is not a valid image format", 
								get_active_function_name(TSRMLS_C), format );
		return;
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageGamma( MagickWand mgck_wnd, float gamma )
*/
ZEND_FUNCTION( magicksetimagegamma )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickSetImageGamma );
}
/* }}} */

/* {{{ proto bool MagickSetImageGreenPrimary( MagickWand mgck_wnd, float x, float y )
*/
ZEND_FUNCTION( magicksetimagegreenprimary )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageGreenPrimary( mgck_wnd, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageIndex( MagickWand mgck_wnd, int index )
*/
ZEND_FUNCTION( magicksetimageindex )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long index;

	MW_GET_2_ARG( "rl", &mgck_wnd_rsrc_zvl_p, &index );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageIndex( mgck_wnd, index ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageInterlaceScheme( MagickWand mgck_wnd, int interlace_type )
*/
ZEND_FUNCTION( magicksetimageinterlacescheme )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( InterlaceType, MagickSetImageInterlaceScheme );
}
/* }}} */

/* {{{ proto bool MagickSetImageIterations( MagickWand mgck_wnd, float iterations )
*/
ZEND_FUNCTION( magicksetimageiterations )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double iterations;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &iterations );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageIterations( mgck_wnd, (unsigned long) iterations ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageMatteColor( MagickWand mgck_wnd, mixed matte_pxl_wnd )
*/
ZEND_FUNCTION( magicksetimagemattecolor )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *matte_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource and a matte color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, matte_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageMatteColor( mgck_wnd, matte_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		matte_pxl_wnd = DestroyPixelWand( matte_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageOption( MagickWand mgck_wnd, string format, string key, string value )
*/
ZEND_FUNCTION( magicksetimageoption )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *format, *key, *value;
	int format_len, key_len, value_len;

	MW_GET_7_ARG( "rsss", &mgck_wnd_rsrc_zvl_p, &format, &format_len, &key, &key_len, &value, &value_len );

	MW_CHECK_PARAM_STR_LEN_EX( (format_len == 0 || key_len == 0 || value_len == 0) );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageOption( mgck_wnd, format, key, value ) );
}
/* }}} */

/* {{{ proto bool MagickSetImagePixels( MagickWand mgck_wnd, int x_offset, int y_offset, float columns, float rows, string map, int storage_type, array pixel_array )
*/
ZEND_FUNCTION( magicksetimagepixels )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_arr, **zvl_pp_element;
	long x_offset, y_offset, php_storage;
	double columns, rows;
	char *map, *c;
	int map_len, pxl_arr_len, i;
	StorageType storage;
	HashPosition pos;

	MW_GET_9_ARG( "rllddsla",
				  &mgck_wnd_rsrc_zvl_p,
				  &x_offset, &y_offset,
				  &columns, &rows,
				  &map, &map_len,
				  &php_storage,
				  &zvl_arr );

	MW_CHECK_PARAM_STR_LEN( map_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	for ( i = 0; i < map_len; i++ ) {
		c = &(map[i]);

		if ( !( *c == 'A' || *c == 'a' ||
				*c == 'B' || *c == 'b' ||
				*c == 'C' || *c == 'c' ||
				*c == 'G' || *c == 'g' ||
				*c == 'I' || *c == 'i' ||
				*c == 'K' || *c == 'k' ||
				*c == 'M' || *c == 'm' ||
				*c == 'O' || *c == 'o' ||
				*c == 'P' || *c == 'p' ||
				*c == 'R' || *c == 'r' ||
				*c == 'Y' || *c == 'y' ) ) {
			MW_SPIT_FATAL_ERR( "map parameter string can only contain the letters A B C G I K M O P R Y; see docs for details" );
			return;
		}
	}

	pxl_arr_len = zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

	if ( pxl_arr_len == 0 ) {
		MW_SPIT_FATAL_ERR( "pixel array parameter was empty" );
		return;
	}

	i = (int) ( columns * rows * map_len );

	if ( pxl_arr_len != i ) {
		zend_error( MW_E_ERROR, "%s(): actual size of pixel array (%d) does not match expected size (%u)",
								get_active_function_name( TSRMLS_C ), pxl_arr_len, i );
		return;
	}

	storage = (StorageType) php_storage;

	i = 0;

#define PRV_SET_PIXELS_ARR_RET_BOOL_2( type )  \
{   type *pxl_arr;  \
	MW_ARR_ECALLOC( type, pxl_arr, pxl_arr_len );  \
	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element )  \
	{  \
		convert_to_double_ex( zvl_pp_element );  \
		pxl_arr[ i++ ] = (type) Z_DVAL_PP( zvl_pp_element );  \
	}  \
	MW_BOOL_FUNC_RETVAL_BOOL(  \
		MagickSetImagePixels( mgck_wnd,  \
							  x_offset, y_offset,  \
							  (unsigned long) columns, (unsigned long) rows,  \
							  map,  \
							  storage,  \
							  (unsigned char *) pxl_arr ) );  \
	efree( pxl_arr );  \
}

	switch ( storage ) {
		 case CharPixel		: PRV_SET_PIXELS_ARR_RET_BOOL_2( unsigned char  );  break;
		 case ShortPixel	: PRV_SET_PIXELS_ARR_RET_BOOL_2( unsigned short );  break;
		 case IntegerPixel	: PRV_SET_PIXELS_ARR_RET_BOOL_2( unsigned int   );  break;
		 case LongPixel		: PRV_SET_PIXELS_ARR_RET_BOOL_2( unsigned long  );  break;
		 case FloatPixel	: PRV_SET_PIXELS_ARR_RET_BOOL_2( float          );  break;
		 case DoublePixel	: PRV_SET_PIXELS_ARR_RET_BOOL_2( double         );  break;

		 default : MW_SPIT_FATAL_ERR( "Invalid StorageType argument supplied to function" );
				   return;
	}
}
/* }}} */

/* {{{ proto bool MagickSetImageProfile( MagickWand mgck_wnd, string name, string profile )
*/
ZEND_FUNCTION( magicksetimageprofile )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *name, *profile;
	int name_len, profile_len;

	MW_GET_5_ARG( "rss", &mgck_wnd_rsrc_zvl_p, &name, &name_len, &profile, &profile_len );

	MW_CHECK_PARAM_STR_LEN_EX( (name_len == 0 || profile_len == 0) );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageProfile( mgck_wnd, name, (unsigned char *) profile, (unsigned long) profile_len ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageRedPrimary( MagickWand mgck_wnd, float x, float y )
*/
ZEND_FUNCTION( magicksetimageredprimary )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageRedPrimary( mgck_wnd, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageRenderingIntent( MagickWand mgck_wnd, int rendering_intent )
*/
ZEND_FUNCTION( magicksetimagerenderingintent )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( RenderingIntent, MagickSetImageRenderingIntent );
}
/* }}} */

/* {{{ proto bool MagickSetImageResolution( MagickWand mgck_wnd, float x_resolution, float y_resolution )
*/
ZEND_FUNCTION( magicksetimageresolution )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x_resolution, y_resolution;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &x_resolution, &y_resolution );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageResolution( mgck_wnd, x_resolution, y_resolution ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageScene( MagickWand mgck_wnd, float scene )
*/
ZEND_FUNCTION( magicksetimagescene )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double scene;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &scene );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageScene( mgck_wnd, (unsigned long) scene ) );
}
/* }}} */

/* {{{ proto bool MagickSetImageType( MagickWand mgck_wnd, int image_type )
*/
ZEND_FUNCTION( magicksetimagetype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( ImageType, MagickSetImageType );
}
/* }}} */

/* {{{ proto bool MagickSetImageUnits( MagickWand mgck_wnd, int resolution_type )
*/
ZEND_FUNCTION( magicksetimageunits )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( ResolutionType, MagickSetImageUnits );
}
/* }}} */

/* {{{ proto bool MagickSetImageVirtualPixelMethod( MagickWand mgck_wnd, int virtual_pixel_method )
*/
ZEND_FUNCTION( magicksetimagevirtualpixelmethod )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( VirtualPixelMethod, MagickSetImageVirtualPixelMethod );
}
/* }}} */

/* {{{ proto bool MagickSetImageWhitePoint( MagickWand mgck_wnd, float x, float y )
*/
ZEND_FUNCTION( magicksetimagewhitepoint )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x, y;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetImageWhitePoint( mgck_wnd, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickSetInterlaceScheme( MagickWand mgck_wnd, InterlaceType interlace_type )
*/
ZEND_FUNCTION( magicksetinterlacescheme )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_ENUM_RET_BOOL( InterlaceType, MagickSetInterlaceScheme );
}
/* }}} */

/* {{{ proto void MagickSetLastIterator( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magicksetlastiterator )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MagickSetLastIterator( mgck_wnd );
}
/* }}} */

/* {{{ proto bool MagickSetPassphrase( MagickWand mgck_wnd, string passphrase )
*/
ZEND_FUNCTION( magicksetpassphrase )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( MagickWand, MagickSetPassphrase );
}
/* }}} */

/* {{{ proto bool MagickSetResolution( MagickWand mgck_wnd, float x_resolution, float y_resolution )
*/
ZEND_FUNCTION( magicksetresolution )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double x_resolution, y_resolution;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &x_resolution, &y_resolution );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetResolution( mgck_wnd, x_resolution, y_resolution ) );
}
/* }}} */

/* {{{ proto bool MagickSetResourceLimit( int resource_type, float limit )
*/
ZEND_FUNCTION( magicksetresourcelimit )
{
	MW_PRINT_DEBUG_INFO

	long rsrc_type;
	double limit;

	MW_GET_2_ARG( "ld", &rsrc_type, &limit );

	MW_CHECK_CONSTANT( ResourceType, rsrc_type );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetResourceLimit( (ResourceType) rsrc_type, (unsigned long) limit ) );
}
/* }}} */

/* {{{ proto bool MagickSetSamplingFactors( MagickWand mgck_wnd, float number_factors, array sampling_factors )
*/
ZEND_FUNCTION( magicksetsamplingfactors )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_arr, **zvl_pp_element;
	int number_factors, i = 0;
	double *sampling_factors;
	HashPosition pos;

	MW_GET_2_ARG( "ra", &mgck_wnd_rsrc_zvl_p, &zvl_arr );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	number_factors = zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

	if ( number_factors < 1 ) {
		RETURN_TRUE;
	}

	MW_ARR_ECALLOC( double, sampling_factors, number_factors );

	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element ) {
		convert_to_double_ex( zvl_pp_element );

		sampling_factors[i++] = Z_DVAL_PP( zvl_pp_element );
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetSamplingFactors( mgck_wnd, (unsigned long) number_factors, sampling_factors ) );

	efree( sampling_factors );
}
/* }}} */

/* {{{ proto bool MagickSetWandSize( MagickWand mgck_wnd, int columns, int rows )
*/
ZEND_FUNCTION( magicksetwandsize )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long columns, rows;

	MW_GET_3_ARG( "rll", &mgck_wnd_rsrc_zvl_p, &columns, &rows );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSetSize( mgck_wnd, (unsigned long) columns, (unsigned long) rows ) );
}
/* }}} */

/* {{{ proto bool MagickShadowImage( MagickWand mgck_wnd, foat opacity, float sigma, int x_offset, int y_offset )
*/
ZEND_FUNCTION( magickshadowimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double opacity, sigma;
	long x_offset, y_offset;

	MW_GET_5_ARG( "rddll", &mgck_wnd_rsrc_zvl_p, &opacity, &sigma, &x_offset, &y_offset );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( MagickShadowImage( mgck_wnd, opacity, sigma, x_offset, y_offset ) == MagickTrue ) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool MagickSharpenImage( MagickWand mgck_wnd, float radius, float sigma [, int channel_type] )
*/
ZEND_FUNCTION( magicksharpenimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma;
	long channel = -1;

	MW_GET_4_ARG( "rdd|l", &mgck_wnd_rsrc_zvl_p, &radius, &sigma, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickSharpenImage( mgck_wnd, radius, sigma ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickSharpenImageChannel( mgck_wnd, (ChannelType) channel, radius, sigma ) );
	}
}
/* }}} */

/* {{{ proto bool MagickShaveImage( MagickWand mgck_wnd, float columns, float rows )
*/
ZEND_FUNCTION( magickshaveimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long columns, rows;

	MW_GET_3_ARG( "rll", &mgck_wnd_rsrc_zvl_p, &columns, &rows );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickShaveImage( mgck_wnd, (unsigned long) columns, (unsigned long) rows ) );
}
/* }}} */

/* {{{ proto bool MagickShearImage( MagickWand mgck_wnd, mixed background_pxl_wnd, float x_shear, float y_shear )
*/
ZEND_FUNCTION( magickshearimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *background_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;
	double x_shear, y_shear;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 4),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource, a background color PixelWand resource "  \
							"(or ImageMagick color string), and x and y shear values" );

	convert_to_double_ex( zvl_pp_args_arr[2] );
	x_shear =  Z_DVAL_PP( zvl_pp_args_arr[2] );
	
	convert_to_double_ex( zvl_pp_args_arr[3] );
	y_shear =  Z_DVAL_PP( zvl_pp_args_arr[3] );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, background_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickShearImage( mgck_wnd, background_pxl_wnd, x_shear, y_shear ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		background_pxl_wnd = DestroyPixelWand( background_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickSolarizeImage( MagickWand mgck_wnd, float threshold )
*/
ZEND_FUNCTION( magicksolarizeimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double threshold;

	MW_GET_2_ARG( "rd", &mgck_wnd_rsrc_zvl_p, &threshold );

	if ( threshold < 0.0 || threshold > MW_MaxRGB ) {
		zend_error( MW_E_ERROR,
					"%s(): value of threshold argument (%0.0f) was invalid. "
					"Threshold value must match \"0 <= threshold <= %0.0f\"",
					get_active_function_name( TSRMLS_C ), threshold, MW_MaxRGB );
		return;
	}

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSolarizeImage( mgck_wnd, threshold ) );
}
/* }}} */

/* {{{ proto bool MagickSpliceImage( MagickWand mgck_wnd, float width, float height, int x, int y )
*/
ZEND_FUNCTION( magickspliceimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long width, height, x, y;

	MW_GET_5_ARG( "rllll", &mgck_wnd_rsrc_zvl_p, &width, &height, &x, &y );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickSpliceImage( mgck_wnd, (unsigned long) width, (unsigned long) height, x, y ) );
}
/* }}} */

/* {{{ proto bool MagickSpreadImage( MagickWand mgck_wnd, float radius )
*/
ZEND_FUNCTION( magickspreadimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickSpreadImage );
}
/* }}} */

/* {{{ proto MagickWand MagickSteganoImage( MagickWand mgck_wnd, MagickWand watermark_wand, int offset )
*/
ZEND_FUNCTION( magicksteganoimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *watermark_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *watermark_wnd_rsrc_zvl_p;
	long offset;

	MW_GET_3_ARG( "rrl", &mgck_wnd_rsrc_zvl_p, &watermark_wnd_rsrc_zvl_p, &offset );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, watermark_wnd, &watermark_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickSteganoImage( mgck_wnd, watermark_wnd, offset ) );
}
/* }}} */

/* {{{ proto MagickWand MagickStereoImage( MagickWand mgck_wnd, MagickWand offset_wand )
*/
ZEND_FUNCTION( magickstereoimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *offset_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *offset_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &offset_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, offset_wnd, &offset_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickStereoImage( mgck_wnd, offset_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickStripImage( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( magickstripimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickStripImage( mgck_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickSwirlImage( MagickWand mgck_wnd, float degrees )
*/
ZEND_FUNCTION( magickswirlimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickSwirlImage );
}
/* }}} */

/* {{{ proto MagickWand MagickTextureImage( MagickWand mgck_wnd, MagickWand texture_wand )
*/
ZEND_FUNCTION( magicktextureimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd, *tex_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *tex_wnd_rsrc_zvl_p;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &tex_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, tex_wnd, &tex_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickTextureImage( mgck_wnd, tex_wnd ) );
}
/* }}} */

/* {{{ proto bool MagickThresholdImage( MagickWand mgck_wnd, float threshold [, int channel_type] )
*/
ZEND_FUNCTION( magickthresholdimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double threshold;
	long channel = -1;

	MW_GET_3_ARG( "rd|l", &mgck_wnd_rsrc_zvl_p, &threshold, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickThresholdImage( mgck_wnd, threshold ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickThresholdImageChannel( mgck_wnd, (ChannelType) channel, threshold ) );
	}
}
/* }}} */

/* {{{ proto bool MagickTintImage( MagickWand mgck_wnd, mixed tint_pxl_wnd, mixed opacity_pxl_wnd )
*/
ZEND_FUNCTION( magicktintimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *tint_pxl_wnd, *opacity_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd, is_script_pxl_wnd_2;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 3),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource and a tint color PixelWand resource (or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, tint_pxl_wnd, is_script_pxl_wnd );

	if ( Z_TYPE_PP( zvl_pp_args_arr[2] ) == IS_RESOURCE ) {

		if (  (   MW_FETCH_RSRC( PixelWand, opacity_pxl_wnd, zvl_pp_args_arr[2] ) == MagickFalse
			   && MW_FETCH_RSRC( PixelIteratorPixelWand, opacity_pxl_wnd, zvl_pp_args_arr[2] ) == MagickFalse )
			|| IsPixelWand( opacity_pxl_wnd ) == MagickFalse )
		{
			MW_SPIT_FATAL_ERR( "invalid resource type as argument #3; a PixelWand resource is required" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				tint_pxl_wnd = DestroyPixelWand( tint_pxl_wnd );
			}
			return;
		}
		is_script_pxl_wnd_2 = 1;
	}
	else {
		is_script_pxl_wnd_2 = 0;

		opacity_pxl_wnd = (PixelWand *) NewPixelWand();

		if ( opacity_pxl_wnd == (PixelWand *) NULL ) {
			MW_SPIT_FATAL_ERR( "unable to create necessary PixelWand" );
			efree( zvl_pp_args_arr );
			if ( !is_script_pxl_wnd ) {
				tint_pxl_wnd = DestroyPixelWand( tint_pxl_wnd );
			}
			return;
		}
		convert_to_string_ex( zvl_pp_args_arr[2] );

		if ( Z_STRLEN_PP( zvl_pp_args_arr[2] ) > 0 ) {

			if ( PixelSetColor( opacity_pxl_wnd, Z_STRVAL_PP( zvl_pp_args_arr[2] ) ) == MagickFalse ) {

				MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	opacity_pxl_wnd, PixelGetException,
													"could not set PixelWand to desired fill color"
				);

				opacity_pxl_wnd = DestroyPixelWand( opacity_pxl_wnd );
				efree( zvl_pp_args_arr );
				if ( !is_script_pxl_wnd ) {
					tint_pxl_wnd = DestroyPixelWand( tint_pxl_wnd );
				}

				return;
			}
		}
	}

	MW_BOOL_FUNC_RETVAL_BOOL( MagickTintImage( mgck_wnd, tint_pxl_wnd, opacity_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		tint_pxl_wnd = DestroyPixelWand( tint_pxl_wnd );
	}
	if ( !is_script_pxl_wnd_2 ) {
		opacity_pxl_wnd = DestroyPixelWand( opacity_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto MagickWand MagickTransformImage( MagickWand mgck_wnd, string crop, string geometry )
*/
ZEND_FUNCTION( magicktransformimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *crop, *geom;
	int crop_len, geom_len;

	MW_GET_5_ARG( "rss", &mgck_wnd_rsrc_zvl_p, &crop, &crop_len, &geom, &geom_len );

	MW_CHECK_PARAM_STR_LEN_EX( (crop_len == 0 || geom_len == 0) );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( MagickWand, MagickTransformImage( mgck_wnd, crop, geom ) );
}
/* }}} */

/* {{{ proto bool MagickTrimImage( MagickWand mgck_wnd, float fuzz )
*/
ZEND_FUNCTION( magicktrimimage )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_MAGICKWAND_SET_DOUBLE_RET_BOOL( MagickTrimImage );
}
/* }}} */

/* {{{ proto bool MagickUnsharpMaskImage( MagickWand mgck_wnd, float radius, float sigma, float amount, float threshold [, int channel_type] )
*/
ZEND_FUNCTION( magickunsharpmaskimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double radius, sigma, amount, threshold;
	long channel = -1;

	MW_GET_6_ARG( "rdddd|l", &mgck_wnd_rsrc_zvl_p, &radius, &sigma, &amount, &threshold, &channel );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	if ( channel == -1 ) {
		MW_BOOL_FUNC_RETVAL_BOOL( MagickUnsharpMaskImage( mgck_wnd, radius, sigma, amount, threshold ) );
	}
	else {
		MW_CHECK_CONSTANT( ChannelType, channel );
		MW_BOOL_FUNC_RETVAL_BOOL( MagickUnsharpMaskImageChannel( mgck_wnd, (ChannelType) channel, radius, sigma, amount, threshold ) );
	}
}
/* }}} */

/* {{{ proto bool MagickWaveImage( MagickWand mgck_wnd, float amplitude, float wave_length )
*/
ZEND_FUNCTION( magickwaveimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	double amplitude, wavelength;

	MW_GET_3_ARG( "rdd", &mgck_wnd_rsrc_zvl_p, &amplitude, &wavelength );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickWaveImage( mgck_wnd, amplitude, wavelength ) );
}
/* }}} */

/* {{{ proto bool MagickWhiteThresholdImage( MagickWand mgck_wnd, mixed threshold_pxl_wnd )
*/
ZEND_FUNCTION( magickwhitethresholdimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	PixelWand *threshold_pxl_wnd;
	zval ***zvl_pp_args_arr;
	int arg_count, is_script_pxl_wnd;

	MW_GET_ARGS_ARRAY_EX(	arg_count, (arg_count != 2),
							zvl_pp_args_arr,
							MagickWand, mgck_wnd,
							"a MagickWand resource and a threshold color PixelWand resource "  \
							"(or ImageMagick color string)" );

	MW_SETUP_PIXELWAND_FROM_ARG_ARRAY( zvl_pp_args_arr, 1, 2, threshold_pxl_wnd, is_script_pxl_wnd );

	MW_BOOL_FUNC_RETVAL_BOOL( MagickWhiteThresholdImage( mgck_wnd, threshold_pxl_wnd ) );

	efree( zvl_pp_args_arr );

	if ( !is_script_pxl_wnd ) {
		threshold_pxl_wnd = DestroyPixelWand( threshold_pxl_wnd );
	}
}
/* }}} */

/* {{{ proto bool MagickWriteImage( MagickWand mgck_wnd, string filename )
*/
ZEND_FUNCTION( magickwriteimage )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *filename_from_script,
		 *filename,
		 *name, *ext,
		 *img_filename,
		 *wand_filename,
		 *tmp_filename,
		 *real_filename,
		 *orig_img_format;
	int filename_from_script_len = 0, field_width;
	php_stream *stream;
	long img_idx;
	size_t wand_filename_len, tmp_filename_len;
	MagickBooleanType img_had_format;

	filename_from_script =
		filename =
		name =
		img_filename =
		wand_filename =
		tmp_filename =
		real_filename = (char *) NULL;

	MW_GET_3_ARG( "r|s", &mgck_wnd_rsrc_zvl_p, &filename_from_script, &filename_from_script_len );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Saves the current active image index, as well as performs a shortcut check for any exceptions,
	   as they will occur if mgck_wnd contains no images
	*/
	img_idx = (long) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_IMAGE_FORMAT( mgck_wnd, img_idx, img_had_format, orig_img_format, "MagickWriteImage" );

	if ( filename_from_script_len > 0 ) {
		/* Point filename string to filename_from_script; make them reference same string */
		filename = filename_from_script;
	}
	else {
		img_filename = (char *) MagickGetImageFilename( mgck_wnd );

		if ( img_filename == (char *) NULL || *img_filename == '\0' ) {

			wand_filename = (char *) MagickGetFilename( mgck_wnd );

			if ( wand_filename == (char *) NULL || *wand_filename == '\0' ) {
				/* No filename at all here, output fatal error, with reason */
				zend_error( MW_E_ERROR, "%s(): the filename argument was empty and, neither the filename of the "  \
										"current image (index %d) nor that of the MagickWand resource argument was set; "  \
										"either supply this function with a filename argument, set the current active image's "  \
										"filename, or, set the MagickWand's filename, BEFORE calling this function",
										get_active_function_name(TSRMLS_C), img_idx );
				MW_FREE_MAGICK_MEM( char *, img_filename  );
				MW_FREE_MAGICK_MEM( char *, wand_filename );
				return;
			}

			/* The following sets up the minimum width of the numbers that will be inserted into the 
			   image's new filename, based on the MagickWand's filename, if the image has no filename.
			   The pattern that the image's new filename will match, will be, if mgck_wnd's filename is
			   "testimage.jpg", and current image index is 5 / 150, and image has no filename, then the
			   image's new filename will be:
			   testimage_005.jpg */
			MW_SET_ZERO_FILLED_WIDTH( mgck_wnd, field_width );

			wand_filename_len = strlen( wand_filename );

			if ( MW_split_filename_on_period( &wand_filename, wand_filename_len,
											  &name, field_width, &ext,
											  &tmp_filename, &tmp_filename_len ) == MagickFalse )
			{
				MW_FREE_MAGICK_MEM( char *, img_filename  );
				MW_FREE_MAGICK_MEM( char *, wand_filename );
				return;
			}

			/* join name to i (made into field_width width), separated by an underscore, then append ext to form the tmp_filename
			*/
			snprintf( tmp_filename, tmp_filename_len, "%s_%.*d%s", name, field_width, img_idx, ext );

			filename = tmp_filename;  /* Point filename string to tmp_filename; make them reference same string */
		}
		else {
			filename = img_filename;  /* Point filename string to img_filename; make them reference same string */
		}
	}

	/* Finally, open a file for writing, obeying all currently implemented php.ini restrictions */
	stream = php_stream_open_wrapper( filename, "wb", MW_STD_STREAM_OPTS, &real_filename );

	if ( stream == (php_stream *) NULL ) {
		/* PHP cannot write image to the current filename: output fatal error, with reason */
		zend_error( MW_E_ERROR, "%s(): PHP cannot write the image at index %d to filename %s; possible php.ini restrictions",
								get_active_function_name(TSRMLS_C), img_idx, filename );
	}
	else {
		if ( MW_write_image_to_php_stream( mgck_wnd, stream, img_idx, real_filename ) == MagickTrue ) {

			/* Check if current image had a format before function started */
			if ( img_had_format == MagickTrue ) {
				RETVAL_TRUE;
			}
			else {
				MW_DEBUG_NOTICE_EX( "Attempting to reset image #%d's image format", img_idx );

				/* If not, it must have been changed to the MagickWand's format, so set it back (probably to nothing) */
				if ( MagickSetImageFormat( mgck_wnd, orig_img_format ) == MagickTrue ) {

					MW_DEBUG_NOTICE_EX(	"SUCCESS: reset image #%d's image format", img_idx );

					RETVAL_TRUE;
				}
				else {
					MW_DEBUG_NOTICE_EX(	"FAILURE: could not reset image #%d's image format", img_idx );

					/* C API could not reset set the current image's format back to its original state: output error, with reason */
					MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
															"unable to set the image at MagickWand index %d back to "  \
															"its original image format",
															img_idx
					);
				}
			}
		}
		php_stream_close( stream );
	}

	MW_FREE_MAGICK_MEM( char *, orig_img_format );

	MW_FREE_MAGICK_MEM( char *, img_filename  );
	MW_FREE_MAGICK_MEM( char *, wand_filename );
	
	MW_EFREE_MEM( char *, name          );
	MW_EFREE_MEM( char *, tmp_filename  );
	MW_EFREE_MEM( char *, real_filename );
}
/* }}} */

/* {{{ proto bool MagickWriteImageFile( MagickWand mgck_wnd, php_stream handle )
*/
ZEND_FUNCTION( magickwriteimagefile )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_stream;
	long img_idx;
	php_stream *stream;
	char *orig_img_format;
	MagickBooleanType img_had_format;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &zvl_stream );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Save the current active image index */
	img_idx = (long) MagickGetImageIndex( mgck_wnd );

	/* Shortcut check for any exceptions -- will occur if mgck_wnd contains no images */
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_IMAGE_FORMAT( mgck_wnd, img_idx, img_had_format, orig_img_format, "MagickWriteImageFile" );

	php_stream_from_zval( stream, &zvl_stream );

	/* You can now use the stream. However, you do not "own" the stream, the script does.
	   That means you MUST NOT close the stream, because it will cause PHP to crash!
	*/

	if ( MW_write_image_to_php_stream( mgck_wnd, stream, img_idx, "the filehandle argument" ) == MagickTrue ) {

		/* Check if current image had a format before function started */
		if ( img_had_format == MagickTrue ) {
			RETVAL_TRUE;
		}
		else {
			MW_DEBUG_NOTICE_EX( "Attempting to reset image #%d's image format", img_idx );

			/* If not, it must have been changed to the MagickWand's format, so set it back (probably to nothing) */
			if ( MagickSetImageFormat( mgck_wnd, orig_img_format ) == MagickTrue ) {

				MW_DEBUG_NOTICE_EX(	"SUCCESS: reset image #%d's image format", img_idx );

				RETVAL_TRUE;
			}
			else {
				MW_DEBUG_NOTICE_EX(	"FAILURE: could not reset image #%d's image format", img_idx );

				/* C API could not reset set the current image's format back to its original state: output error, with reason */
				MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
														"unable to set the image at MagickWand index %d back to "  \
														"its original image format",
														img_idx
				);
			}
		}
	}
	else {
		RETVAL_FALSE;
	}
	
	MW_FREE_MAGICK_MEM( char *, orig_img_format );
}
/* }}} */

/* {{{ proto bool MagickWriteImages( MagickWand mgck_wnd [, string filename [, bool join_images]] )
*/
ZEND_FUNCTION( magickwriteimages )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	char *filename_from_script,
		 *wand_filename,
		 *wand_format,
		 *img_format,
		 *filename,
		 *name, *ext,
		 *img_filename,
		 *tmp_img_filename,
		 *new_img_filename,
		 *real_img_filename,
		 *orig_img_format;
	int filename_from_script_len = 0;
	zend_bool join_images = FALSE;
	php_stream *stream;
	long initial_index, cur_img_idx;
	int field_width;
	size_t filename_len = 0, tmp_img_filename_len;
	MagickBooleanType img_had_format;

	filename_from_script =
		wand_filename =
		wand_format =
		img_format =
		filename =
		name =
		img_filename =
		tmp_img_filename =
		new_img_filename =
		real_img_filename = (char *) NULL;

	MW_GET_4_ARG( "r|sb",
				  &mgck_wnd_rsrc_zvl_p,
				  &filename_from_script, &filename_from_script_len,
				  &join_images );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Saves the current active image index, plus performs shortcut check for any exceptions,
	   which will occur if mgck_wnd contains no images
	*/
	initial_index = (long) MagickGetImageIndex( mgck_wnd );

	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	if ( filename_from_script != (char *) NULL ) {
		filename = filename_from_script;
		filename_len = (size_t) filename_from_script_len;
	}
	else {
		wand_filename = (char *) MagickGetFilename( mgck_wnd );

		if ( !( wand_filename == (char *) NULL || *wand_filename == '\0' ) ) {
			filename = wand_filename;
			filename_len = strlen( filename );
		}
	}

	if ( join_images == TRUE ) {
		if ( filename_len < 1 ) {
			/* No filename at all here, output fatal error, with reason */
			MW_SPIT_FATAL_ERR( "the filename sent to the function was empty, and the MagickWand resource does not "  \
							   "have a filename set; the MagickWand's images cannot be joined and written as 1 image" );
		}
		else {
			wand_format = (char *) MagickGetFormat( mgck_wnd );

			if ( wand_format == (char *) NULL || *wand_format == '\0' || *wand_format == '*' ) {
				MW_SPIT_FATAL_ERR(	"the MagickWand resource sent to this function did not have an image format set "  \
									"(via MagickSetFormat()); the MagickWand's image format must be set in order "  \
									"for MagickWriteImages() to continue with the join_images argument (the "  \
									"optional 3rd argument) as FALSE (its default)" );
			}
			else {
				/* Try opening the file for appending -- while this has the side effects of
				   1) being slower than testing
						the file's stat info
						&& whether safe mode and other php.ini restrictions are on
						&& whether the filename passes said restrictions,
					  I have not yet researched those possibilities, so this kludge will have to do
				   2) creating the file if it does not already exist (which only matters if the script
					  exits before ImageMagick can write to the file, or if Imagemagick CAN'T write to the
					  file, which shouldn't occur if it passess this test :)
				   these side effects are "palatable" for the benefit of making sure that the filename is
				   valid by user's php.ini (security) standards.
				*/
				stream = php_stream_open_wrapper( filename, "wb", MW_STD_STREAM_OPTS, &real_img_filename );

				if ( stream == (php_stream *) NULL ) {
					/* PHP cannot write image to the current filename: output fatal error, with reason */
					zend_error( MW_E_ERROR, "%s(): PHP cannot write images to filename %s; possible php.ini restrictions",
											get_active_function_name(TSRMLS_C), filename );
				}
				else {
					MW_DEBUG_NOTICE_EX( "Attempting to write images to file \"%s\"", real_img_filename );

					if ( MW_write_images_to_php_stream( mgck_wnd, stream, real_img_filename ) == MagickTrue ) {
						MW_DEBUG_NOTICE_EX( "Wrote images to file \"%s\"", real_img_filename );
						RETVAL_TRUE;
					}
					else {
						MW_DEBUG_NOTICE_EX( "Error writing images to file \"%s\"", real_img_filename );
					}
					php_stream_close( stream );
				}

				MW_EFREE_MEM( char *, real_img_filename );
			}

			MW_FREE_MAGICK_MEM( char *, wand_format );
		}

		MW_FREE_MAGICK_MEM( char *, wand_filename );
		return;
	}
	else {
		/* Check if there is a valid filename "registered", and if so, prepare the parts of the
		   filenames to be substituted if any images don't have filenames:
		   name + _ + cur_img_idx (field_width fixed width) + ext (e.g.: testimage_08.jpg).
		   If all the images have filenames, this never matters; if any don't, this comes into effect.
		   If any images don't have filenames, and the filename_len is <= 0 (i.e. no "external"
		   filename, an error will occur, and the script will exit.
		*/
		if ( filename_len > 0 ) {
			/* The following sets up the minimum width of the numbers that will be inserted into the 
			   image's new filename, based on the MagickWand's filename, if the image has no filename.
			   The pattern that the image's new filename will match, will be, if mgck_wnd's filename is
			   "testimage.jpg", and current image index is 5 / 150, and image has no filename, then the
			   image's new filename will be:
			   testimage_005.jpg
			*/
			MW_SET_ZERO_FILLED_WIDTH( mgck_wnd, field_width );

			MW_DEBUG_NOTICE_EX( "Setting up \"%s\" for incrementing...", filename );

			if ( MW_split_filename_on_period( &filename, filename_len,
											  &name, field_width, &ext,
											  &tmp_img_filename, &tmp_img_filename_len ) == MagickFalse )
			{
				MW_FREE_MAGICK_MEM( char *, wand_filename );
				return;
			}

			MW_DEBUG_NOTICE( "magickwriteimages(): MW_split_filename_on_period() function completed successfully" );
			MW_DEBUG_NOTICE_EX( "magickwriteimages(): name = \"%s\"", (name == (char *) NULL ? "NULL" : name) );
			MW_DEBUG_NOTICE_EX( "magickwriteimages(): ext = \"%s\"", ext );
			MW_DEBUG_NOTICE_EX( "magickwriteimages(): tmp_img_filename_len = %d", tmp_img_filename_len );
		}

		MW_DEBUG_NOTICE_EX( "The initial image index was %d", initial_index );

		MW_DEBUG_NOTICE( "Resetting MagickWand's iterator" );
		MagickResetIterator( mgck_wnd ); /* Start from the beginning... */

		MW_DEBUG_NOTICE( "Getting MagickWand's image format" );
		wand_format = (char *) MagickGetFormat( mgck_wnd );

		for ( cur_img_idx = 0; MagickNextImage( mgck_wnd ) == MagickTrue; cur_img_idx++ ) {

			MW_DEBUG_NOTICE_EX( "Checking image #%d for an image format", cur_img_idx );

			img_format = (char *) MagickGetImageFormat( mgck_wnd );
			if ( img_format == (char *) NULL || *img_format == '\0' || *img_format == '*' ) {

				MW_DEBUG_NOTICE_EX( "Image #%d has no image format", cur_img_idx );

				MW_FREE_MAGICK_MEM( char *, img_format );

				img_had_format = MagickFalse;

				if ( wand_format == (char *) NULL || *wand_format == '\0' || *wand_format == '*' ) {
					zend_error( MW_E_ERROR, "neither the MagickWand resource sent to this function, nor its current "  \
											"active image (index %d) had an image format set (via MagickSetFormat() or "  \
											"MagickSetImageFormat()); the function checks for the current active "  \
											"image's image format, and then for the MagickWand's image format -- "  \
											"one of them must be set in order for MagickWriteImages() to continue",
											get_active_function_name(TSRMLS_C), cur_img_idx );
					break;
				}
				else {
					MW_DEBUG_NOTICE_EX( "Attempting to set image #%d's image format to the MagickWand's image format",
										cur_img_idx );

					if ( MagickSetImageFormat( mgck_wnd, wand_format ) == MagickTrue ) {

						MW_DEBUG_NOTICE_EX(	"SUCCESS: set image #%d's image format to the MagickWand's image format",
											cur_img_idx );

						orig_img_format = (char *) NULL;
					}
					else {
						MW_DEBUG_NOTICE_EX(	"FAILURE: could not set image #%d's image format to the MagickWand's image format",
											cur_img_idx );

						/* C API cannot set the current image's format to the MagickWand's format: output error, with reason */
						MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_2(	mgck_wnd, MagickGetException,
																"unable to set the format of the image at index %d "  \
																"to the MagickWand's set image format \"%s\"",
																cur_img_idx, wand_format
						);

						break;
					}
				}
			}
			else {
				MW_DEBUG_NOTICE_EX( "Image #%d had an image format", cur_img_idx );

				img_had_format = MagickTrue;

				orig_img_format = img_format;
			}

			MW_DEBUG_NOTICE_EX( "Setting up image #%d's filename", cur_img_idx );

			/* If there was a filename argument, or a MagickWand filename... */
			if ( filename_len > 0 ) {

				MW_DEBUG_NOTICE( "A filename argument or MagickWand filename exists, so:" );

				/* ... join name to cur_img_idx (made into field_width width), separated by an
				   underscore, then append ext to form the tmp_img_filename
				*/
				snprintf( tmp_img_filename, tmp_img_filename_len, "%s_%*.*ld%s", name, field_width, field_width, cur_img_idx, ext );

				MW_DEBUG_NOTICE_EX( "tmp_img_filename = \"%s\"", tmp_img_filename );

				/* Point new_img_filename string to tmp_img_filename; make them reference same string */
				new_img_filename = tmp_img_filename;
			}
			else { /* ... otherwise check each image for a filename */
				MW_DEBUG_NOTICE( "NO filename argument or MagickWand filename exists, so..." );

				img_filename = (char *) MagickGetImageFilename( mgck_wnd );

				if ( img_filename == (char *) NULL || *img_filename == '\0' ) {
					/* Bad img_filename; new_img_filename gets set to an invalid string */
					new_img_filename = (char *) NULL;

					MW_DEBUG_NOTICE( "there is NO image filename" );
				}
				else {
					/* Point new_img_filename string to img_filename; make them reference same string */
					new_img_filename = img_filename;
					
					MW_DEBUG_NOTICE_EX( "img_filename = \"%s\"", img_filename );
				}
			}

			MW_DEBUG_NOTICE_EX( "new_img_filename = \"%s\"", (new_img_filename == (char *) NULL ? "NULL" : new_img_filename) );

			/* Is the new_img_filename valid? */

			/* If the new_img_filename is INVALID... */
			if ( new_img_filename == (char *) NULL ) {
				/* No filename at all here, output error, with reason */
				zend_error( MW_E_ERROR, "%s(): the filename sent to this function was empty and, neither "  \
										"the image at MagickWand index %d's filename nor the MagickWand "  \
										"resource's filename was set; either supply this function with a "  \
										"filename argument or, set the image's filename or the "  \
										"MagickWand's filename, BEFORE calling this function",
										get_active_function_name(TSRMLS_C), cur_img_idx );

				MW_FREE_MAGICK_MEM( char *, orig_img_format );
				MW_FREE_MAGICK_MEM( char *, img_filename );

				break;
			}
			else { /* the new_img_filename is valid */
				MW_DEBUG_NOTICE_EX( "new_img_filename = \"%s\"", new_img_filename );

				/* Finally, try to open a test file for appending (writing w/o file truncation),
				   to see if php.ini whould allow file to be written to the newly incremented filename */
				stream = php_stream_open_wrapper( new_img_filename, "wb", MW_STD_STREAM_OPTS, &real_img_filename );

				if ( stream == (php_stream *) NULL ) {
					/* PHP cannot write image to the current filename: output fatal error, with reason */
					zend_error( MW_E_ERROR, "%s(): PHP cannot write the image at index %d to filename %s; "  \
											"possible php.ini restrictions",
											get_active_function_name(TSRMLS_C), cur_img_idx, new_img_filename );

					MW_EFREE_MEM( char *, real_img_filename );

					MW_FREE_MAGICK_MEM( char *, orig_img_format );
					MW_FREE_MAGICK_MEM( char *, img_filename );

					break;
				}
				else {
					MW_DEBUG_NOTICE_EX( "PHP successfully opened \"%s\"", new_img_filename );

					if ( MW_write_image_to_php_stream( mgck_wnd, stream, cur_img_idx, real_img_filename ) == MagickTrue ) {

						MW_DEBUG_NOTICE_EX( "Wrote images to file \"%s\"", real_img_filename );

						/* Check if current image had a format before function started */
						if ( img_had_format == MagickFalse ) {

							MW_DEBUG_NOTICE_EX( "Attempting to reset image #%d's image format", cur_img_idx );

							/* If not, it must have been changed to the MagickWand's format, so set it back (probably to nothing) */
							if ( MagickSetImageFormat( mgck_wnd, orig_img_format ) == MagickFalse ) {

								MW_DEBUG_NOTICE_EX(	"FAILURE: could not reset image #%d's image format", cur_img_idx );

								/* C API could not reset set the current image's format back to its original state:
								   output error, with reason
								*/
								MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
																		"unable to set the image at MagickWand index %d back to "  \
																		"its original image format",
																		cur_img_idx
								);

								php_stream_close( stream );

								MW_EFREE_MEM( char *, real_img_filename );

								MW_FREE_MAGICK_MEM( char *, orig_img_format );
								MW_FREE_MAGICK_MEM( char *, img_filename );

								break;
							}
						}
					}
					else {
						php_stream_close( stream );

						MW_EFREE_MEM( char *, real_img_filename );

						MW_FREE_MAGICK_MEM( char *, orig_img_format );
						MW_FREE_MAGICK_MEM( char *, img_filename );

						break;
					}
					php_stream_close( stream );
				}
				MW_EFREE_MEM( char *, real_img_filename );
			}
			MW_FREE_MAGICK_MEM( char *, orig_img_format );
			MW_FREE_MAGICK_MEM( char *, img_filename );
		}

		MW_EFREE_MEM( char *, name );
		MW_EFREE_MEM( char *, tmp_img_filename  );

		MW_FREE_MAGICK_MEM( char *, wand_format );

		MW_FREE_MAGICK_MEM( char *, wand_filename );

		/* If all the above was successful, set the image index back to what it was */
		if ( MagickSetImageIndex( mgck_wnd, initial_index ) == MagickFalse ) {

			/* Just in case error occurs in MagickSetImageIndex() */
			MW_API_FUNC_FAIL_CHECK_WAND_ERROR_EX_1(	mgck_wnd, MagickGetException,
													"cannot reset the MagickWand index to %d",
													initial_index
			);
		}
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool MagickWriteImagesFile( MagickWand mgck_wnd, php_stream handle )
*/
ZEND_FUNCTION( magickwriteimagesfile )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p, *zvl_stream;
	php_stream *stream;

	MW_GET_2_ARG( "rr", &mgck_wnd_rsrc_zvl_p, &zvl_stream );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	/* Shortcut check for any exceptions -- will occur if mgck_wnd contains no images */
	(void) MagickGetImageIndex( mgck_wnd );
	if ( mgck_wnd->exception.severity != UndefinedException  ) {
		RETURN_FALSE;
	}
	MagickClearException( mgck_wnd );

	MW_CHECK_FOR_WAND_FORMAT( mgck_wnd, "MagickWriteImagesFile" );

	php_stream_from_zval( stream, &zvl_stream );

	/* You can now use the stream.  However, you do not "own" the stream, the script does.
	   That means you MUST NOT close the stream, because it will cause PHP to crash!
	*/

	if ( MW_write_images_to_php_stream( mgck_wnd, stream, "the filehandle argument" ) == MagickTrue ) {
		RETURN_TRUE;
	}
}
/* }}} */



/*
****************************************************************************************************
*************************     PixelIterator Functions     *************************
****************************************************************************************************
*/

/* {{{ proto void ClearPixelIterator( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( clearpixeliterator )
{
	MW_PRINT_DEBUG_INFO

	PixelIterator *pxl_iter;
	zval *pxl_iter_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &pxl_iter_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( PixelIterator, pxl_iter, &pxl_iter_rsrc_zvl_p );

	ClearPixelIterator( pxl_iter );
}
/* }}} */

/* {{{ proto void DestroyPixelIterator( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( destroypixeliterator )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_RSRC_DESTROY_POINTER( PixelIterator );
}
/* }}} */

/* {{{ proto PixelIterator NewPixelIterator( MagickWand mgck_wnd )
*/
ZEND_FUNCTION( newpixeliterator )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &mgck_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( PixelIterator, NewPixelIterator( mgck_wnd ) );
}
/* }}} */

/* {{{ proto PixelIterator NewPixelRegionIterator( MagickWand mgck_wnd, int x, int y, int columns, int rows )
*/
ZEND_FUNCTION( newpixelregioniterator )
{
	MW_PRINT_DEBUG_INFO

	MagickWand *mgck_wnd;
	zval *mgck_wnd_rsrc_zvl_p;
	long x, y, columns, rows;

	MW_GET_5_ARG( "rllll", &mgck_wnd_rsrc_zvl_p, &x, &y, &columns, &rows );

	MW_GET_POINTER_FROM_RSRC( MagickWand, mgck_wnd, &mgck_wnd_rsrc_zvl_p );

	MW_SET_RET_RSRC_FROM_FUNC( PixelIterator, NewPixelRegionIterator( mgck_wnd, x, y, (unsigned long) columns, (unsigned long) rows ) );
}
/* }}} */

/* {{{ proto array PixelGetIteratorException( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( pixelgetiteratorexception )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_ARR( PixelIterator );
}
/* }}} */

/* {{{ proto string PixelGetIteratorExceptionString( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( pixelgetiteratorexceptionstring )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_STR( PixelIterator );
}
/* }}} */

/* {{{ proto int PixelGetIteratorExceptionType( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( pixelgetiteratorexceptiontype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_TYPE( PixelIterator );
}
/* }}} */

/* {{{ proto array PixelGetNextIteratorRow( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( pixelgetnextiteratorrow )
{
	MW_PRINT_DEBUG_INFO

/*  The following is only used in this function to ensure that the right kind of PixelWand array
	is returned to the script, i.e. an array of PixelIterator PixelWands that WILL not be auto
	cleaned up by PHP, but which will only b destroyed when the pxl_iter is cleaned up by
	ImageMagick
*/
	MW_CHECK_ERR_RET_PixelIteratorPixelWand_ARR( PixelGetNextIteratorRow );
}
/* }}} */

/* {{{ proto void PixelResetIterator( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( pixelresetiterator )
{
	MW_PRINT_DEBUG_INFO

	PixelIterator *pxl_iter;
	zval *pxl_iter_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &pxl_iter_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( PixelIterator, pxl_iter, &pxl_iter_rsrc_zvl_p );

	PixelResetIterator( pxl_iter );
}
/* }}} */

/* {{{ proto bool PixelSetIteratorRow( PixelIterator pxl_iter, int row )
*/
ZEND_FUNCTION( pixelsetiteratorrow )
{
	MW_PRINT_DEBUG_INFO

	PixelIterator *pxl_iter;
	zval *pxl_iter_rsrc_zvl_p;
	long row;

	MW_GET_2_ARG( "rl", &pxl_iter_rsrc_zvl_p, &row );

	MW_GET_POINTER_FROM_RSRC( PixelIterator, pxl_iter, &pxl_iter_rsrc_zvl_p );

	MW_BOOL_FUNC_RETVAL_BOOL( PixelSetIteratorRow( pxl_iter, row ) );
}
/* }}} */

/* {{{ proto bool PixelSyncIterator( PixelIterator pxl_iter )
*/
ZEND_FUNCTION( pixelsynciterator )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_FUNC_BOOL( PixelIterator, PixelSyncIterator );
}
/* }}} */


/*
****************************************************************************************************
*************************     PixelWand Functions     *************************
****************************************************************************************************
*/

/* {{{ proto void ClearPixelWand( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( clearpixelwand )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	zval *pxl_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &pxl_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( PixelWand, pxl_wnd, &pxl_wnd_rsrc_zvl_p );

	ClearPixelWand( pxl_wnd );
}
/* }}} */

/* {{{ proto void DestroyPixelWand( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( destroypixelwand )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	zval *pxl_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &pxl_wnd_rsrc_zvl_p );

	/*  This specifically for the DestroyPixelWand*() functions -- can't have them trying to destroy
		PixelIterator PixelWands
	*/
	MW_DESTROY_ONLY_PixelWand( pxl_wnd, &pxl_wnd_rsrc_zvl_p );
}
/* }}} */

/* {{{ proto void DestroyPixelWandArray( array pxl_wnd_array )
*/
ZEND_FUNCTION( destroypixelwandarray )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	zval *zvl_arr, **zvl_pp_element;
	int num_wnds;
	HashPosition pos;

	MW_GET_1_ARG( "a", &zvl_arr );

	num_wnds = zend_hash_num_elements( Z_ARRVAL_P( zvl_arr ) );

	if ( num_wnds < 1 ) {
		MW_SPIT_FATAL_ERR( "array argument must contain at least 1 PixelWand resource" );
	}

	/* Makes sure no attempts to destroy PixelIterator PixelWands are made by explicitly 
	   retrieving only "pure" (as far as PHP is concerned) PixelWands
	*/
	MW_ITERATE_OVER_PHP_ARRAY( pos, zvl_arr, zvl_pp_element ) {
		if ( MW_FETCH_RSRC( PixelWand, pxl_wnd, zvl_pp_element ) == MagickFalse || IsPixelWand( pxl_wnd ) == MagickFalse ) {
			zend_error( MW_E_ERROR, "%s(): function can only act on an array of PixelWand resources; "  \
									"(NOTE: PixelWands derived from PixelIterators are also invalid)",
									get_active_function_name(TSRMLS_C) );
			return;
		}
		zend_list_delete( Z_LVAL_PP( zvl_pp_element ) );
	}
/*
	PixelWand *pxl_wnd;
	int arg_count = ZEND_NUM_ARGS(), i;
	zval ***zvl_pp_args_arr, **zvl_pp_element;
	HashPosition pos;

	if ( arg_count < 1 ) {
		zend_error( MW_E_ERROR, "%s(): error in function call: function requires at least 1 parameter -- "  \
								"at least 1 of either (or both) of the following: PixelWand resources, "  \
								"or arrays of PixelWand resources",
								get_active_function_name(TSRMLS_C) );
		return;
	}

	MW_ARR_ECALLOC( zval **, zvl_pp_args_arr, arg_count );

	if ( zend_get_parameters_array_ex( arg_count, zvl_pp_args_arr ) == FAILURE ) {
		zend_error( MW_E_ERROR, "%s(): unknown error in function call", get_active_function_name(TSRMLS_C) );
		efree( zvl_pp_args_arr );
		return;
	}

#define PRV_DESTROY_THE_PIXELWAND( pxl_wnd_rsrc_zvl_pp )  \
{   if ( MW_FETCH_RSRC( PixelWand, pxl_wnd, pxl_wnd_rsrc_zvl_pp ) == MagickFalse || IsPixelWand( pxl_wnd ) == MagickFalse ) {  \
		zend_error( MW_E_ERROR,  \
					"%s(): function can act only on PixelWand resources or arrays of PixelWand resources; "  \
					"argument #%d was neither. "  \
					"(NOTE: PixelWands derived from PixelIterators are also invalid)",  \
					get_active_function_name(TSRMLS_C), i );  \
		efree( zvl_pp_args_arr );  \
		return;  \
	}  \
	zend_list_delete( Z_LVAL_PP( zvl_pp_args_arr[i] ) );  \
}

	for ( i = 0; i < arg_count; i++ ) {
		switch ( Z_TYPE_PP( zvl_pp_args_arr[i] ) ) {
			case IS_RESOURCE : PRV_DESTROY_THE_PIXELWAND( zvl_pp_args_arr[i] );
							   break;

			case IS_ARRAY    : MW_ITERATE_OVER_PHP_ARRAY( pos, *(zvl_pp_args_arr[i]), zvl_pp_element ) {
									PRV_DESTROY_THE_PIXELWAND( zvl_pp_element );
							   }
							   break;

			default          : MW_SPIT_FATAL_ERR(
									"argument(s) must either be individual PixelWand resources or arrays of Pixelwand resources" );
							   efree( zvl_pp_args_arr );
							   return;
		}
	}
	efree( zvl_pp_args_arr );
*/
}
/* }}} */

/* {{{ proto PixelWand NewPixelWand( [string imagemagick_col_str] )
*/
ZEND_FUNCTION( newpixelwand )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *new_pxl_wnd;
	char *col_str;
	int col_str_len = 0;

	MW_GET_2_ARG( "|s", &col_str, &col_str_len );

	new_pxl_wnd = (PixelWand *) NewPixelWand();

	MW_SET_RET_RSRC( PixelWand, new_pxl_wnd );

	if ( col_str_len > 0 && PixelSetColor( new_pxl_wnd, col_str ) == MagickFalse ) {

		MW_API_FUNC_FAIL_CHECK_WAND_ERROR(	new_pxl_wnd, PixelGetException,
											"could not set PixelWand to desired fill color"
		);
	}
}
/* }}} */

/* {{{ proto array NewPixelWandArray( int num_wnds )
*/
ZEND_FUNCTION( newpixelwandarray )
{
	MW_PRINT_DEBUG_INFO

	long num_wnds_req;
	unsigned long num_wnds;
	PixelWand **pxl_wnd_arr = (PixelWand **) NULL;

	MW_GET_1_ARG( "l", &num_wnds_req );

	if ( num_wnds_req < 1 ) {
		MW_SPIT_FATAL_ERR( "user must request 1 or more PixelWand resources" );
		return;
	}

	num_wnds = (unsigned long) num_wnds_req;

	pxl_wnd_arr = (PixelWand **) NewPixelWands( num_wnds );

	if ( pxl_wnd_arr == (PixelWand **) NULL ) {
		RETURN_FALSE;
	}
	else {
		MW_RET_RESOURCE_ARR( PixelWand, pxl_wnd_arr, num_wnds, PixelWand );
	}
}
/* }}} */

/* {{{ proto float PixelGetAlpha( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetalpha )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetAlpha );
}
/* }}} */

/* {{{ proto float PixelGetAlphaQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetalphaquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetAlphaQuantum );
}
/* }}} */

/* {{{ proto array PixelGetException( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetexception )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_ARR( PixelWand );
}
/* }}} */

/* {{{ proto string PixelGetExceptionString( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetexceptionstring )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_STR( PixelWand );
}
/* }}} */

/* {{{ proto int PixelGetExceptionType( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetexceptiontype )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_EXCEPTION_TYPE( PixelWand );
}
/* }}} */

/* {{{ proto float PixelGetBlack( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetblack )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetBlack );
}
/* }}} */

/* {{{ proto float PixelGetBlackQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetblackquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetBlackQuantum );
}
/* }}} */

/* {{{ proto float PixelGetBlue( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetblue )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetBlue );
}
/* }}} */

/* {{{ proto float PixelGetBlueQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetbluequantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetBlueQuantum );
}
/* }}} */

/* {{{ proto string PixelGetColorAsString( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetcolorasstring )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RETVAL_STRING( PixelWand, PixelGetColorAsString );
}
/* }}} */

/* {{{ proto float PixelGetColorCount( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetcolorcount )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetColorCount );
}
/* }}} */

/* {{{ proto float PixelGetCyan( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetcyan )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetCyan );
}
/* }}} */

/* {{{ proto float PixelGetCyanQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetcyanquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetCyanQuantum );
}
/* }}} */

/* {{{ proto float PixelGetGreen( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetgreen )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetGreen );
}
/* }}} */

/* {{{ proto float PixelGetGreenQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetgreenquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetGreenQuantum );
}
/* }}} */

/* {{{ proto float PixelGetIndex( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetindex )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetIndex );
}
/* }}} */

/* {{{ proto float PixelGetMagenta( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetmagenta )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetMagenta );
}
/* }}} */

/* {{{ proto float PixelGetMagentaQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetmagentaquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetMagentaQuantum );
}
/* }}} */

/* {{{ proto float PixelGetOpacity( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetopacity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetOpacity );
}
/* }}} */

/* {{{ proto float PixelGetOpacityQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetopacityquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetOpacityQuantum );
}
/* }}} */

/* {{{ proto array PixelGetQuantumColor( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetquantumcolor )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	PixelPacket *pxl_pckt;
	zval *pxl_wnd_rsrc_zvl_p;

	MW_GET_1_ARG( "r", &pxl_wnd_rsrc_zvl_p );

	MW_GET_POINTER_FROM_RSRC( PixelWand, pxl_wnd, &pxl_wnd_rsrc_zvl_p );

	MW_EMALLOC( PixelPacket, pxl_pckt );

	PixelGetQuantumColor( pxl_wnd, pxl_pckt );

	array_init( return_value );

	if (   add_assoc_double( return_value, "r", (double) pxl_pckt->red     ) == FAILURE
		|| add_assoc_double( return_value, "g", (double) pxl_pckt->green   ) == FAILURE
		|| add_assoc_double( return_value, "b", (double) pxl_pckt->blue    ) == FAILURE
		|| add_assoc_double( return_value, "o", (double) pxl_pckt->opacity ) == FAILURE )
	{
		MW_SPIT_FATAL_ERR( "error adding a value to the return array" );
	}

	efree( pxl_pckt );
}
/* }}} */

/* {{{ proto float PixelGetRed( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetred )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetRed );
}
/* }}} */

/* {{{ proto float PixelGetRedQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetredquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetRedQuantum );
}
/* }}} */

/* {{{ proto float PixelGetYellow( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetyellow )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetYellow );
}
/* }}} */

/* {{{ proto float PixelGetYellowQuantum( PixelWand pxl_wnd )
*/
ZEND_FUNCTION( pixelgetyellowquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_RET_DOUBLE( PixelWand, PixelGetYellowQuantum );
}
/* }}} */

/* {{{ proto void PixelSetAlpha( PixelWand pxl_wnd, float alpha )
*/
ZEND_FUNCTION( pixelsetalpha )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetAlpha );
}
/* }}} */

/* {{{ proto void PixelSetAlphaQuantum( PixelWand pxl_wnd, float alpha )
*/
ZEND_FUNCTION( pixelsetalphaquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetAlphaQuantum );
}
/* }}} */

/* {{{ proto void PixelSetBlack( PixelWand pxl_wnd, float black )
*/
ZEND_FUNCTION( pixelsetblack )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetBlack );
}
/* }}} */

/* {{{ proto void PixelSetBlackQuantum( PixelWand pxl_wnd, float black )
*/
ZEND_FUNCTION( pixelsetblackquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetBlackQuantum );
}
/* }}} */

/* {{{ proto void PixelSetBlue( PixelWand pxl_wnd, float blue )
*/
ZEND_FUNCTION( pixelsetblue )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetBlue );
}
/* }}} */

/* {{{ proto void PixelSetBlueQuantum( PixelWand pxl_wnd, float blue )
*/
ZEND_FUNCTION( pixelsetbluequantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetBlueQuantum );
}
/* }}} */

/* {{{ proto bool PixelSetColor( PixelWand pxl_wnd, string imagemagick_col_str )
*/
ZEND_FUNCTION( pixelsetcolor )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_AND_STRING_RETVAL_FUNC_BOOL( PixelWand, PixelSetColor );
}
/* }}} */

/* {{{ proto void PixelSetColorCount( PixelWand pxl_wnd, int count )
*/
ZEND_FUNCTION( pixelsetcolorcount )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	zval *pxl_wnd_rsrc_zvl_p;
	long count;

	MW_GET_2_ARG( "rl", &pxl_wnd_rsrc_zvl_p, &count );

	MW_GET_POINTER_FROM_RSRC( PixelWand, pxl_wnd, &pxl_wnd_rsrc_zvl_p );

	PixelSetColorCount( pxl_wnd, (unsigned long) count );
}
/* }}} */

/* {{{ proto void PixelSetCyan( PixelWand pxl_wnd, float cyan )
*/
ZEND_FUNCTION( pixelsetcyan )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetCyan );
}
/* }}} */

/* {{{ proto void PixelSetCyanQuantum( PixelWand pxl_wnd, float cyan )
*/
ZEND_FUNCTION( pixelsetcyanquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetCyanQuantum );
}
/* }}} */

/* {{{ proto void PixelSetGreen( PixelWand pxl_wnd, float green )
*/
ZEND_FUNCTION( pixelsetgreen )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetGreen );
}
/* }}} */

/* {{{ proto void PixelSetGreenQuantum( PixelWand pxl_wnd, float green )
*/
ZEND_FUNCTION( pixelsetgreenquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetGreenQuantum );
}
/* }}} */

/* {{{ proto void PixelSetIndex( PixelWand pxl_wnd, float index )
*/
ZEND_FUNCTION( pixelsetindex )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	zval *pxl_wnd_rsrc_zvl_p;
	double index;

	MW_GET_2_ARG( "rd", &pxl_wnd_rsrc_zvl_p, &index );

	if ( index < 0.0 || index > MW_MaxRGB ) {
		zend_error( MW_E_ERROR, "%s(): the value of the colormap index argument (%d) was invalid. "  \
								"The colormap index value must match \"0 <= index <= %0.0f\"",
								get_active_function_name( TSRMLS_C ), index, MW_MaxRGB );
		return;
	}

	MW_GET_POINTER_FROM_RSRC( PixelWand, pxl_wnd, &pxl_wnd_rsrc_zvl_p );

	PixelSetIndex( pxl_wnd, (IndexPacket) index );
}
/* }}} */

/* {{{ proto void PixelSetMagenta( PixelWand pxl_wnd, float magenta )
*/
ZEND_FUNCTION( pixelsetmagenta )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetMagenta );
}
/* }}} */

/* {{{ proto void PixelSetMagentaQuantum( PixelWand pxl_wnd, float magenta )
*/
ZEND_FUNCTION( pixelsetmagentaquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetMagentaQuantum );
}
/* }}} */

/* {{{ proto void PixelSetOpacity( PixelWand pxl_wnd, float opacity )
*/
ZEND_FUNCTION( pixelsetopacity )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetOpacity );
}
/* }}} */

/* {{{ proto void PixelSetOpacityQuantum( PixelWand pxl_wnd, float opacity )
*/
ZEND_FUNCTION( pixelsetopacityquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetOpacityQuantum );
}
/* }}} */

/* {{{ proto void PixelSetQuantumColor( PixelWand pxl_wnd, float red, float green, float blue [, float opacity] )
*/
ZEND_FUNCTION( pixelsetquantumcolor )
{
	MW_PRINT_DEBUG_INFO

	PixelWand *pxl_wnd;
	PixelPacket *pxl_pckt;
	zval *pxl_wnd_rsrc_zvl_p;
	double red, green, blue, opacity = 0;

	MW_GET_5_ARG( "rddd|d", &pxl_wnd_rsrc_zvl_p, &red, &green, &blue, &opacity );

	if (   red     < 0 || red     > MW_MaxRGB
		|| green   < 0 || green   > MW_MaxRGB
		|| blue    < 0 || blue    > MW_MaxRGB
		|| opacity < 0 || opacity > MW_MaxRGB )
	{
		zend_error( MW_E_ERROR, "%s(): the value of one or more of the Quantum color arguments was invalid. "  \
								"Quantum color values must match \"0 <= color_val <= %0.0f\"",
								get_active_function_name( TSRMLS_C ), MW_MaxRGB );
		return;
	}

	MW_GET_POINTER_FROM_RSRC( PixelWand, pxl_wnd, &pxl_wnd_rsrc_zvl_p );

	MW_EMALLOC( PixelPacket, pxl_pckt );

	pxl_pckt->red     = (Quantum) red;
	pxl_pckt->green   = (Quantum) green;
	pxl_pckt->blue    = (Quantum) blue;
	pxl_pckt->opacity = (Quantum) opacity;

	PixelSetQuantumColor( pxl_wnd, pxl_pckt );

	efree( pxl_pckt );
}
/* }}} */

/* {{{ proto void PixelSetRed( PixelWand pxl_wnd, float red )
*/
ZEND_FUNCTION( pixelsetred )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetRed );
}
/* }}} */

/* {{{ proto void PixelSetRedQuantum( PixelWand pxl_wnd, float red )
*/
ZEND_FUNCTION( pixelsetredquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetRedQuantum );
}
/* }}} */

/* {{{ proto void PixelSetYellow( PixelWand pxl_wnd, float yellow )
*/
ZEND_FUNCTION( pixelsetyellow )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_WAND_SET_NORMALIZED_COLOR( PixelWand, PixelSetYellow );
}
/* }}} */

/* {{{ proto void PixelSetYellowQuantum( PixelWand pxl_wnd, float yellow )
*/
ZEND_FUNCTION( pixelsetyellowquantum )
{
	MW_PRINT_DEBUG_INFO

	MW_GET_PIXELWAND_SET_QUANTUM_COLOR( PixelSetYellowQuantum );
}
/* }}} */

