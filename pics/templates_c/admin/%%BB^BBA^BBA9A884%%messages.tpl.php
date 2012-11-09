<?php /* Smarty version 2.6.3, created on 2005-06-04 01:59:36
         compiled from ../modules/messages/messages.tpl */ ?>
<?php if ($this->_tpl_vars['error'] == ""):  else: ?>
<div class="errmsg"> 
<?php echo $this->_tpl_vars['error']; ?>

</div>
<?php endif; ?>
                                                                                                                                                               
<?php if ($this->_tpl_vars['success'] != ""): ?>
<div class="scsmsg"> 
<?php echo $this->_tpl_vars['success']; ?>

</div>
<?php else:  endif; ?>