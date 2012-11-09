<?php /* Smarty version 2.6.3, created on 2005-06-04 01:59:38
         compiled from ../modules/http_upload/http_upload.tpl */ ?>
<?php if ($this->_tpl_vars['firstPage'] == 'true'): ?>
	<form name="chooseuser" action=<?php echo $this->_tpl_vars['action']; ?>
 method="POST">
	<table class="forms">
	<tr>
	    <td class="formch">  </td>
	    <td class="formc"> Name   </td>
	    <td class="formc"> Email/UserName  </td>
	    <td class="formc"> userId  </td>
	</tr>
	<?php if (count($_from = (array)$this->_tpl_vars['users'])):
    foreach ($_from as $this->_tpl_vars['index']):
?>
	<tr>
	    <td class="formch"><input type="radio" name="uid" value="<?php echo $this->_tpl_vars['index']['userId']; ?>
"></td>
	    <td class="formc"> <?php echo $this->_tpl_vars['index']['lname']; ?>
, <?php echo $this->_tpl_vars['index']['fname']; ?>
 <?php echo $this->_tpl_vars['index']['mname']; ?>
 </td>
	    <td class="formc"> <?php echo $this->_tpl_vars['index']['email']; ?>
                       </td>
	    <td class="formc"> <?php echo $this->_tpl_vars['index']['userId']; ?>
                      </td>
	</tr>
	<?php endforeach; unset($_from); endif; ?>
	</table>
                                                                                                                                                                    
	<table class="buttons">
	<tr>
	<td class="buttons">
	<a class="button" href="javascript:document.chooseuser.submit(); " title="Insert pics for selected user"> Submit </a>
	</td>
	</tr>
	</table>
	</form>
<?php else: ?>
	<?php if ($this->_tpl_vars['success'] == ""): ?>
	<form name="pics" method="POST" action=<?php echo $this->_tpl_vars['action']; ?>
 enctype="multipart/form-data">
	<input type="hidden" name="uid" value="<?php echo $this->_tpl_vars['uid']; ?>
">
	<table class="buttons">
	<tr>
	<td class="buttons">
		<input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons" colspan="2">
		<!-- <input type="submit" value="Upload Files"> -->
		<a class="button" href="javascript:document.pics.submit(); " title="Insert pics for selected user"> Submit </a>
	</td>
	</tr>
	</table>
	</form>
	<?php endif;  endif; ?>