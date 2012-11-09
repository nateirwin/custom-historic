<?php /* Smarty version 2.6.3, created on 2005-07-25 12:30:22
         compiled from ../modules/users/edit_user.tpl */ ?>
<?php if ($this->_tpl_vars['firstPage'] == 'true'): ?>
	<table class="forms">
	<tr>
	    <td class="formc"> Name   </td>
	    <td class="formc"> Email/UserName  </td>
	</tr>
	<?php if (count($_from = (array)$this->_tpl_vars['users'])):
    foreach ($_from as $this->_tpl_vars['index']):
?>
	<tr>
	    <td class="formc"> <a href="edit.php?uid=<?php echo $this->_tpl_vars['index']['userId']; ?>
&db=true" title="Click here to edit user account" > <?php echo $this->_tpl_vars['index']['fname']; ?>
 <?php echo $this->_tpl_vars['index']['mname']; ?>
 <?php echo $this->_tpl_vars['index']['lname']; ?>
 </a> </td>
	    <td class="formc"> <?php echo $this->_tpl_vars['index']['email']; ?>
                       </td>
	</tr>
	<?php endforeach; unset($_from); endif; ?>
	</table>
<?php else: ?>
	<form name="edituser" action=<?php echo $this->_tpl_vars['action']; ?>
 method="POST">
	<table class="forms">
	<tr>
	<td class="forml"> Title:
	</td>
	<td class="formr"> 
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['title']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="title" value="<?php echo $this->_tpl_vars['title']; ?>
" size="40" maxlength="35" >
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> *First Name:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['fname']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="fname" value="<?php echo $this->_tpl_vars['fname']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> MI:
	</td>
	<td class="formr"> 
		<?php if ($this->_tpl_vars['success'] != ""): ?>	
		<?php echo $this->_tpl_vars['updated']['mname']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="mname" value="<?php echo $this->_tpl_vars['mname']; ?>
" size="40" maxlength="35">
	<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> *Last Name:
	</td>
	<td class="formr"> 
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['lname']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="lname" value="<?php echo $this->_tpl_vars['lname']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> Address:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['address']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="address" value="<?php echo $this->_tpl_vars['address']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> City:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['city']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="city" value="<?php echo $this->_tpl_vars['city']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> State:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['state']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="state" value="<?php echo $this->_tpl_vars['state']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> Zip:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['zip']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="zip" value="<?php echo $this->_tpl_vars['zip']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> Home Tel:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['homeTel']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="htel" value="<?php echo $this->_tpl_vars['htel']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<td class="forml"> Work Tel:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['workTel']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="wtel" value="<?php echo $this->_tpl_vars['wtel']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<td class="forml"> Cell Tel:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['cellTel']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="ctel" value="<?php echo $this->_tpl_vars['ctel']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> Contact Person:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['contactPerson']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="ct" value="<?php echo $this->_tpl_vars['ct']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> Contact Tel:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['contactTel']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="ctt" value="<?php echo $this->_tpl_vars['ctt']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> *Password:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['userPass']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="passwd" value="<?php echo $this->_tpl_vars['passwd']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
	<tr>
	<td class="forml"> * Email/Unique User Id:
	</td>
	<td class="formr">
		<?php if ($this->_tpl_vars['success'] != ""): ?>
		<?php echo $this->_tpl_vars['updated']['email']; ?>

		<?php else: ?>
		<input class="txtbox" type="text" name="email" value="<?php echo $this->_tpl_vars['email']; ?>
" size="40" maxlength="35">
		<?php endif; ?>
	</td>
	</tr>
</table>

	<?php if ($this->_tpl_vars['success'] != ""): ?>
	<?php else: ?>
	<table class="buttons">
	<tr>
	<td class="buttons">
	<a class="button" href="javascript:document.edituser.submit(); " title="Click here to Edit the User Information"> Submit </a>
	</td>
	</tr>
	</table>
	<?php endif; ?>
</form>
<?php endif; ?>