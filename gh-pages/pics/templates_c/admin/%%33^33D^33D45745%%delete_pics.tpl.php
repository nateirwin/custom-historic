<?php /* Smarty version 2.6.3, created on 2005-07-26 19:20:40
         compiled from ../modules/pics/delete_pics.tpl */ ?>
<script language="JavaScript" SRC="templates/js/popup.js">
</script>
<form name="deletepics" method="POST">
<table class="buttons">
<?php if (count($_from = (array)$this->_tpl_vars['userPics'])):
    foreach ($_from as $this->_tpl_vars['index']):
?>
<tr>
<td>
<div class="pics">
<input type="checkbox" name="<?php echo $this->_tpl_vars['index']['pic']; ?>
" value="<?php echo $this->_tpl_vars['index']['pic']; ?>
"> <a href="./images/pop_<?php echo $this->_tpl_vars['index']['pic']; ?>
" onclick="newWindow(this.href,'this.name','yes'); return false;" title="Enlarge Picture"> <img alt="thumbnail" src="./images/thb_<?php echo $this->_tpl_vars['index']['pic']; ?>
"> </a>
</div>
<div class="admincomments">
<?php echo $this->_tpl_vars['index']['adminComment']; ?>

</div>
<div class="usercomments">
<?php echo $this->_tpl_vars['index']['userComment']; ?>

</div>
<div class="clear">
</div>
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
<a class="button" href="javascript:document.deletepics.submit(); " title="Click here to Delete Selected Pictures"> Submit </a>
</td>
</tr>
</table>
<?php endif; ?>
</form>
