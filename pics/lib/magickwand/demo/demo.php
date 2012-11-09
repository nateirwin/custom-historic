<?php

/*
* MagickWand for PHP :: Demonstration program
*
* Author: Ouinnel Watson
* Date: December 2004
*
* WARNING
*   By installing and executing this script, you accept any and all
*   responsibility for any and all damages of any kind which occur as
*   a result of the installation and/or operation of this script, and
*   agree to indemnify the author (Ouinnel Watson) against any liability
*   and/or responsibility for any and all damages resulting from the
*   installation, execution or operation of this script.
*
* Description: The following script demonstrates the inate capabilities of
*              MagickWand for PHP by creating an animated GIF.
*              MagickWand for PHP capabilities used:
*              1) vector drawing (shapes, text, colors)
*              2) default font
*              3) unique color model
*              4) addition of random noise
*              5) several special effects methods
*              6) setting of single frame display time
*              7) and much more...
*/

$noise_frames = 5; 
$noise_delay = 20; 

$cnt_down_start = 3; 
$cnt_down_delay = 50; 

$width = 140; 
$height = 70; 

$x_int = $width / 10; 
$y_int = $height / 10; 

$x1 = $x_int; 
$y1 = $y_int; 

$x2 = $width - $x_int; 
$y2 = $height - $y_int; 

$x_radius = $y_radius = 10; 

$font_size = 30; 

$swirl_deg_start = 0; 
$swirl_deg_end = 360; 
$swirl_deg_int = 60; 
$swirl_delay = 10; 

$num_morph_frames = 5; 
$morph_delay = 20; 

$logo = 'MagickWand for PHP'; 

$welcome_msg = 'The future awaits...'; 

$mgck_wnd = NewMagickWand(); 

$bg_color= NewPixelWand(); 

$white = NewPixelWand('white'); 

$pxl_wnd = NewPixelWand();

$drw_wnd = NewDrawingWand(); 

$txt_wnd = NewDrawingWand();

for ($i = 0; $i < $noise_frames; $i++) {

    checkWandError(MagickNewImage($mgck_wnd, $width, $height, $bg_color), $mgck_wnd, __LINE__);

    checkWandError(MagickAddNoiseImage($mgck_wnd, MW_LaplacianNoise), $mgck_wnd, __LINE__);

    checkWandError(MagickSetImageDelay($mgck_wnd, $noise_delay), $mgck_wnd, __LINE__);
}

$pre_morph_wnd =& checkWandError(MagickGetImage($mgck_wnd), $mgck_wnd, __LINE__);

$red   = mt_rand(0, 255);
$green = mt_rand(0, 255);
$blue  = mt_rand(0, 255);

checkWandError(PixelSetColor($pxl_wnd, "rgb($red, $green, $blue)"), $pxl_wnd, __LINE__);

DrawSetFillColor($drw_wnd, $pxl_wnd);

DrawRoundRectangle($drw_wnd, $x1, $y1, $x2, $y2, $x_radius, $y_radius);

drawNewImageSetDelay($pre_morph_wnd, $drw_wnd, $width, $height, $bg_color, $morph_delay, __LINE__);

addMorphedImages($mgck_wnd, $pre_morph_wnd, $num_morph_frames, $morph_delay, __LINE__);

DestroyMagickWand($pre_morph_wnd);

MagickSetLastIterator($mgck_wnd);

for ($i = $cnt_down_start; $i > 0 && ($x2 - $x1) >= $x_int && ($y2 - $y1) >= $y_int; $i--) {

    checkWandError(PixelSetColor($pxl_wnd, "rgb($red, $green, $blue)"), $pxl_wnd, __LINE__);

    DrawSetFillColor($drw_wnd, $pxl_wnd);
    DrawRoundRectangle($drw_wnd, $x1, $y1, $x2, $y2, $x_radius, $y_radius);

    $red   = mt_rand(0, 255);
    $green = mt_rand(0, 255);
    $blue  = mt_rand(0, 255);

    $x1 += $x_int;
    $y1 += $y_int;

    $x2 -= $x_int;
    $y2 -= $y_int;

    drawNewImageSetDelay($mgck_wnd, $drw_wnd, $width, $height, $bg_color, $cnt_down_delay, __LINE__);

    addText($txt_wnd, $white, $font_size, $i);

    checkWandError(MagickDrawImage($mgck_wnd, $txt_wnd), $mgck_wnd, __LINE__);

    ClearDrawingWand($txt_wnd);
}

addText($drw_wnd, $white, $font_size, $i);

drawNewImageSetDelay($mgck_wnd, $drw_wnd, $width, $height, $bg_color, $swirl_delay, __LINE__);

for ($swirl_deg = $swirl_deg_start; $swirl_deg <= $swirl_deg_end; $swirl_deg += $swirl_deg_int) {

    drawNewImageSetDelay($mgck_wnd, $drw_wnd, $width, $height, $bg_color, $swirl_delay, __LINE__);

    checkWandError(MagickSwirlImage($mgck_wnd, $swirl_deg), $mgck_wnd, __LINE__);
}

while (TRUE) {

    DrawSetFontSize($txt_wnd, $font_size);

    $logo_width =& checkWandError(MagickGetStringWidth($mgck_wnd, $txt_wnd, $logo), $mgck_wnd, __LINE__);
    $welcome_msg_width =& checkWandError(MagickGetStringWidth($mgck_wnd, $txt_wnd, $welcome_msg), $mgck_wnd, __LINE__);

    if ($logo_width < $width && $welcome_msg_width < $width) {
        break;
    }

    $font_size--;
}

$pre_morph_wnd =& checkWandError(MagickGetImage($mgck_wnd), $mgck_wnd, __LINE__);

addText($txt_wnd, $white, $font_size, $logo);
drawNewImage($pre_morph_wnd, $txt_wnd, $width, $height, $bg_color, __LINE__);

ClearDrawingWand($txt_wnd);

addText($txt_wnd, $white, $font_size, $welcome_msg);
drawNewImage($pre_morph_wnd, $txt_wnd, $width, $height, $bg_color, __LINE__);

MagickSetFirstIterator($mgck_wnd);
checkWandError(MagickAddImage($pre_morph_wnd, $mgck_wnd), $pre_morph_wnd, __LINE__);

addMorphedImages($mgck_wnd, $pre_morph_wnd, $num_morph_frames, $morph_delay, __LINE__);

DestroymagickWand($pre_morph_wnd);
DestroyDrawingWand($drw_wnd);
DestroyDrawingWand($txt_wnd);

checkWandError(MagickSetFormat($mgck_wnd, 'GIF'), $mgck_wnd, __LINE__);

header('Content-Type: ' . MagickGetMimeType($mgck_wnd));

MagickEchoImagesBlob($mgck_wnd);

DestroymagickWand($mgck_wnd);

/* ********** Function Declarations ********** */

/**
 * Function checkWandError() compares the value of $result, which should be
 * the result of any MagickWand API function, to explicit FALSE, and if it is
 * FALSE, checks if $wand for contains an error condition (in case the API
 * function is just returning FALSE as a normal return value).
 *
 * If the return value is FALSE, and the $wand contains a set error condition,
 * the function outputs the error and forcibly ends the program.
 *
 * If not, returns $result as a reference.
 *
 * @param mixed       MagickWand API function result
 * @param resource    Any MagickWand API resource
 * @param int         Always __LINE__ (script current line number predefined
 *                        PHP constant)
 *
 * @return reference  Returns reference to 1st argument (if no errors found)
 */
function &checkWandError(&$result, $wand, $line) {

    if ($result === FALSE && WandHasException($wand)) {
        echo '<pre>An error occurred on line ', $line, ': ', WandGetExceptionString($wand), '</pre>';
        exit();
    }

    return $result;
}

/**
 * Function addText() performs several operations on the DrawingWand
 * $drw_wnd:
 *    1) sets the fill color (the color in which shapes, text, etc. will be
 *       drawn) from the PixelWand (or Imagemagick color string) $pxl_wnd with
 *       DrawSetFillColor()
 *    2) sets the font size of text to be drawn to $font_size, using
 *       DrawSetFontSize()
 *    3) sets the position where text will be drawn with DrawSetGravity();
 *       position is set to MW_CenterGravity by default, which automatically
 *       centers text horizontally and vertically
 *    4) sets text to be drawn later to $text, which will be drawn at
 *       coordinate ($x, $y), (relative to the position indicated by
 *       $gravity), with DrawAnnotation()
 *
 * If no font is set prior to this function being called, the MagickWand API
 * uses the default font (seems to be Arial).
 *
 * @param resource    DrawingWand resource
 * @param mixed       PixelWand resource or imagemagick color string
 * @param float       desired text font size
 * @param float       string to be drawn
 * @param int         the desired text gravity, indicating the desired text
 *                        position; must be a MagickWand API GravityType
 * @param int         x ordinate, relative to the chosen text gravity where
 *                        text will be drawn
 * @param int         y ordinate, relative to the chosen text gravity where
 *                        text will be drawn
 *
 * @return void       No return value, as all functions used return void.
 */
function addText($drw_wnd, $pxl_wnd, $font_size, $text, $gravity = MW_CenterGravity, $x = 0, $y = 0) {

    DrawSetFillColor($drw_wnd, $pxl_wnd);

    DrawSetFontSize($drw_wnd, $font_size);

    DrawSetGravity($drw_wnd, $gravity);

    DrawAnnotation($drw_wnd, $x, $y, $text);
}

/**
 * Function drawNewImage() creates a new image, using MagickNewImage(), of
 * $width * $height area, filled with $bg_color (a PixelWand, or ImageMagick
 * color string) color.
 *
 * It then uses MagickDrawImage() to draw the commands contained in the
 * DrawingWand $drw_wnd on the newly reated image.
 *
 * @param resource    MagickWand resource
 * @param resource    DrawingWand resource
 * @param int         width of new image
 * @param int         height of new image
 * @param mixed       PixelWand resource or imagemagick color string
 * @param int         Always __LINE__ (script current line number predefined
 *                        PHP constant)
 *
 * @return void
 */
function drawNewImage($mgck_wnd, $drw_wnd, $width, $height, $bg_color, $line) {

    $line = 'program line '.$line.', function line ';

    checkWandError(MagickNewImage($mgck_wnd, $width, $height, $bg_color), $mgck_wnd, $line.__LINE__);
    checkWandError(MagickDrawImage($mgck_wnd, $drw_wnd), $mgck_wnd, $line.__LINE__);
}

/**
 * Function drawNewImageSetDelay() calls drawNewImage() to create a new
 * image, of $width * $height area, filled with $bg_color color, drawn on by
 * DrawingWand $drw_wnd.
 *
 * It then sets the length of time the new image will be displayed to $delay.
 *
 * @param resource    MagickWand resource
 * @param resource    DrawingWand resource
 * @param int         width of new image
 * @param int         height of new image
 * @param mixed       PixelWand resource or imagemagick color string
 * @param int         desired length of time (in hundredths of a second) that
 *                        the new image will be displayed
 * @param int         Always __LINE__ (script current line number predefined
 *                        PHP constant)
 *
 * @return void
 */
function drawNewImageSetDelay($mgck_wnd, $drw_wnd, $width, $height, $bg_color, $delay, $line) {

    drawNewImage($mgck_wnd, $drw_wnd, $width, $height, $bg_color, $line);

    $line = 'program line '.$line.', function line ';

    checkWandError(MagickSetImageDelay($mgck_wnd, $delay), $mgck_wnd, $line.__LINE__);
}

/**
 * Function addMorphedImages() morphs the images in $pre_morph_wnd, with
 * $num_morph_frames intermediary frames. The new sequence's images are then
 * set to display for $morph_delay 100th's of a second, and then added to the
 * end of $mgck_wnd's image list.
 *
 * @param resource    MagickWand resource
 * @param resource    MagickWand resource
 * @param int         number of intermediary frames desired between the images
 *                        to be morphed
 * @param int         desired length of time (in hundredths of a second) that
 *                        each frame in the newly morphed sequence will be
 *                        displayed
 * @param int         Always __LINE__ (script current line number predefined
 *                        PHP constant)
 *
 * @return void
 */
function addMorphedImages($mgck_wnd, $pre_morph_wnd, $num_morph_frames, $morph_delay, $line) {

    $line = 'program line '.$line.', function line ';

    MagickSetFirstIterator($pre_morph_wnd);

    $morph_wnd =& checkWandError(MagickMorphImages($pre_morph_wnd, $num_morph_frames), $pre_morph_wnd, $line.__LINE__);

    MagickSetFirstIterator($morph_wnd);
    checkWandError(MagickRemoveImage($morph_wnd), $morph_wnd, $line.__LINE__);

    MagickSetLastIterator($morph_wnd);
    checkWandError(MagickRemoveImage($morph_wnd), $morph_wnd, $line.__LINE__);

    MagickResetIterator($morph_wnd);
    while (MagickNextImage($morph_wnd)) {
            checkWandError(MagickSetImageDelay($morph_wnd, $morph_delay), $morph_wnd, $line.__LINE__);
    }

    MagickSetLastIterator($morph_wnd);

    MagickSetLastIterator($mgck_wnd);
    checkWandError(MagickAddImages($mgck_wnd, $morph_wnd), $mgck_wnd, $line.__LINE__);

    DestroymagickWand($morph_wnd);
}

?>
