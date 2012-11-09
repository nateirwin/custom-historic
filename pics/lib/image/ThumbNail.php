<?php

class ThumbNail {
	
	function ThumbNail($imgfile) {

		//find the image format
		$this->img["format"]=ereg_replace(".*\.(.*)$","\\1",$imgfile);
		$this->img["format"]=strtoupper($this->img["format"]);

		if ($this->img["format"]=="JPG" || $this->img["format"]=="JPEG") {
			//JPEG
			$this->img["format"]="JPEG";
			$this->img["src"] = ImageCreateFromJPEG ($imgfile);
		} elseif ($this->img["format"]=="PNG") {
			//PNG
			$this->img["format"]="PNG";
			$this->img["src"] = ImageCreateFromPNG ($imgfile);
		} elseif ($this->img["format"]=="GIF") {
			//GIF
			$this->img["format"]="GIF";
			$this->img["src"] = ImageCreateFromGIF ($imgfile);
		} elseif ($this->img["format"]=="WBMP") {
			//WBMP
			$this->img["format"]="WBMP";
			$this->img["src"] = ImageCreateFromWBMP ($imgfile);
		} else {
			//DEFAULT
			echo "Not Supported File";
			exit();
		}
		@$this->img["lebar"] = imagesx($this->img["src"]);
		@$this->img["tinggi"] = imagesy($this->img["src"]);
		//default quality jpeg
		$this->img["quality"]=75;
	}
	
	/*
	function sizeHeight($size=100)
	{
		//height
		$this->img["tinggi_thumb"]=$size;
		@$this->img["lebar_thumb"] = ($this->img["tinggi_thumb"]/$this->img["tinggi"])*$this->img["lebar"];
	}

	function sizeWidth($size=100)
	{
		//width
		$this->img["lebar_thumb"]=$size;
		@$this->img["tinggi_thumb"] = ($this->img["lebar_thumb"]/$this->img["lebar"])*$this->img["tinggi"];
	}
	*/
	function sizeAuto( $size=100 ) {
		if ($this->img["lebar"]>=$this->img["tinggi"]) {
			$this->img["lebar_thumb"]=$size;
			@$this->img["tinggi_thumb"] = ($this->img["lebar_thumb"]/$this->img["lebar"])*$this->img["tinggi"];
		} else {
			$this->img["tinggi_thumb"]=$size;
			@$this->img["lebar_thumb"] = ($this->img["tinggi_thumb"]/$this->img["tinggi"])*$this->img["lebar"];
		}
	}

	function save( $save ) {
		/* change ImageCreateTrueColor to ImageCreate if your GD not supported ImageCreateTrueColor function*/
		$this->img["des"] = ImageCreateTrueColor($this->img["lebar_thumb"],$this->img["tinggi_thumb"]);
		@imagecopyresized ($this->img["des"], $this->img["src"], 0, 0, 0, 0, $this->img["lebar_thumb"], $this->img["tinggi_thumb"], $this->img["lebar"], $this->img["tinggi"]);

		if ($this->img["format"]=="JPG" || $this->img["format"]=="JPEG") {
			//JPEG
			imageJPEG($this->img["des"],"$save",$this->img["quality"]);
		} elseif ($this->img["format"]=="PNG") {
			//PNG
			imagePNG($this->img["des"],"$save");
		} elseif ($this->img["format"]=="GIF") {
			//GIF
			imageGIF($this->img["des"],"$save");
		} elseif ($this->img["format"]=="WBMP") {
			//WBMP
			imageWBMP($this->img["des"],"$save");
		}
	}

	function build ($picName) {
	    //foreach ( $userPictures as $userPicArr ) {
            //$picName = $userPicArr['pic'];
            $pic = "./images/$picName";
            $thumb = "./images/thb_$picName";
            $popUp = "./images/pop_$picName";
            $thumbNail = new ThumbNail($pic);
            $this->sizeAuto(75);
            $this->save($thumb);
            $this->sizeAuto(864);
            $this->save($popUp);
		//}
	}

}/*end class*/

?>

