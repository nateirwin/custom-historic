<?php /* Smarty version 2.6.3, created on 2005-07-26 19:34:28
         compiled from ../modules/pics/add_comments_users.tpl */ ?>
<form name="addcomments" method="POST">
<table class="buttons">
<?php if (count($_from = (array)$this->_tpl_vars['userPics'])):
    foreach ($_from as $this->_tpl_vars['index']):
?>
<tr>
<td class="pics">
<a href="./images/pop_<?php echo $this->_tpl_vars['index']['pic']; ?>
" onclick="newWindow(this.href,'this.name','yes'); return false;" title="Enlarge Picture"> <img alt="thumbnail" src="./images/thb_<?php echo $this->_tpl_vars['index']['pic']; ?>
"> </a>
</td>
<td class="pics">
<textarea name="<?php echo $this->_tpl_vars['index']['pic']; ?>
" cols="40" rows="3"><?php echo $this->_tpl_vars['index']['userComment']; ?>
</textarea> 
</td>
</tr>
<?php endforeach; unset($_from); else:  if ($this->_tpl_vars['uid'] >= 1): ?>
<tr>
<td>
No pictures available for the selected User
</td>
</tr>
<?php else: ?>
<tr>
<td>
Select a user and click on the submit button
</td>
</tr>
<?php endif;  endif; ?>
</table>

<?php if ($this->_tpl_vars['userPics'] != null): ?>
<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.addcomments.submit(); " title="Click here to add comments to the Selected Pictures"> Submit </a>
</td>
</tr>
</table>
<?php endif; ?>
</form>