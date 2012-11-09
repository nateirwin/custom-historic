<?php /* Smarty version 2.6.3, created on 2005-06-29 01:15:02
         compiled from ../modules/users/list_users.tpl */ ?>
<form name="listuser" method="POST">
<table class="buttons">
<tr>
<td class="buttons">
<select name="uid">
<option value="0"> Select a email/User Id </option>
<?php if (count($_from = (array)$this->_tpl_vars['userList'])):
    foreach ($_from as $this->_tpl_vars['index']):
?>
<option value="<?php echo $this->_tpl_vars['index']['userId']; ?>
"> <?php echo $this->_tpl_vars['index']['fname']; ?>
 <?php echo $this->_tpl_vars['index']['mname']; ?>
 <?php echo $this->_tpl_vars['index']['lname']; ?>
 , <?php echo $this->_tpl_vars['index']['email']; ?>
 </option>
<?php endforeach; unset($_from); endif; ?>
</select>
</td>
</tr>
</table>

<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.listuser.submit(); " title="View selected user's pictures"> Submit </a>
</td>
</tr>
</table>
</form>