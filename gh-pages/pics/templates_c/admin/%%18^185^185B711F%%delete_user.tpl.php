<?php /* Smarty version 2.6.3, created on 2006-07-16 14:13:55
         compiled from ../modules/users/delete_user.tpl */ ?>
<form name="deleteuser" method="POST">
<table class="forms">
<tr>
	<td class="formch">  </td>
    <td class="formc"> Name   </td>
    <td class="formc"> Email/UserName  </td>
</tr>
<?php if (count($_from = (array)$this->_tpl_vars['users'])):
    foreach ($_from as $this->_tpl_vars['index']):
?>
<tr>
	<td class="formch"><input type="checkbox" name="<?php echo $this->_tpl_vars['index']['userId']; ?>
" value="<?php echo $this->_tpl_vars['index']['userId']; ?>
"></td>
    <td class="formc"> <?php echo $this->_tpl_vars['index']['fname']; ?>
 <?php echo $this->_tpl_vars['index']['mname']; ?>
 <?php echo $this->_tpl_vars['index']['lname']; ?>
 </td>
    <td class="formc"> <?php echo $this->_tpl_vars['index']['email']; ?>
                       </td>
</tr>
<?php endforeach; unset($_from); endif; ?>
</table>

<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.deleteuser.submit(); " title="Click here to Delete New User"> Submit </a>
</td>
</tr>
</table>
</form>