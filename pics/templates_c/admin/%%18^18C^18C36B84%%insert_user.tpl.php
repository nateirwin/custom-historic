<?php /* Smarty version 2.6.3, created on 2005-06-29 01:14:58
         compiled from ../modules/users/insert_user.tpl */ ?>
<form name="insert" method="POST">
<table class="forms">
<tr>
<td class="forml"> Title:
</td>
<td class="formr"> 
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['title']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['fname']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['mname']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['lname']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['address']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['city']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['state']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['zip']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['homeTel']; ?>

<?php else: ?>
<input class="txtbox" type="text" name="htel" value="<?php echo $this->_tpl_vars['htel']; ?>
" size="40" maxlength="35">
<?php endif; ?>
</td>
</tr>
<td class="forml"> Work Tel:
</td>
<td class="formr">
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['workTel']; ?>

<?php else: ?>
<input class="txtbox" type="text" name="wtel" value="<?php echo $this->_tpl_vars['wtel']; ?>
" size="40" maxlength="35">
<?php endif; ?>
</td>
</tr>
<td class="forml"> Cell Tel:
</td>
<td class="formr">
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['cellTel']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['contactPerson']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['contactTel']; ?>

<?php else: ?>
<input class="txtbox" type="text" name="ctt" value="<?php echo $this->_tpl_vars['ctt']; ?>
" size="40" maxlength="35">
<?php endif; ?>
</td>
</tr>
<tr>
<td class="forml"> *Password:</td>
<td class="formr">
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['userPass']; ?>

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
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  echo $this->_tpl_vars['inserted']['email']; ?>

<?php else: ?>
<input class="txtbox" type="text" name="email" value="<?php echo $this->_tpl_vars['email']; ?>
" size="40" maxlength="35">
<?php endif; ?>
</td>
</tr>
<?php if ($this->_tpl_vars['inFlag'] == 'true'):  else: ?>
<tr>
<td class="forml"> * Email/Unique User Id:
</td>
<td class="formr">
<input class="txtbox" type="text" name="verify" value="<?php echo $this->_tpl_vars['verify']; ?>
" size="40" maxlength="35">
</td>
</tr>
<?php endif; ?>
</table>

<?php if ($this->_tpl_vars['inFlag'] == 'true'):  else: ?>
<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.insert.submit(); " title="Click here to Insert New User"> Submit </a>
</td>
</tr>
</table>
<?php endif; ?>
</form>