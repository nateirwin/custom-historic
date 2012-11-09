{if $success != "" }
{else}
<form name="updatePass" method="POST">
<table class="forms">
<tr>
<td class="forml"> * Old Password: </td>
<td class="formr"> <input class="txtbox" type="password" name="oldpasswd" size="40" maxlength="35"> </td>
</tr>
<tr>
<td class="forml"> * New Password: </td>
<td class="formr"> <input class="txtbox" type="password" name="passwd" size="40" maxlength="35"> </td>
</tr>
<tr>
<td class="forml"> * New Password: </td>
<td class="formr"> <input class="txtbox" type="password" name="verify" size="40" maxlength="35"> </td>
</tr>
</table>

<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.updatePass.submit(); " title="Click here to update your password"> Submit </a>
</td>
</tr>
</table>
{/if}
</form>
