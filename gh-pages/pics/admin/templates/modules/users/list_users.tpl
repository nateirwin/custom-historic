<form name="listuser" method="POST">
<table class="buttons">
<tr>
<td class="buttons">
<select name="uid">
<option value="0"> Select a email/User Id </option>
{foreach item=index from=$userList}
<option value="{$index.userId}"> {$index.fname} {$index.mname} {$index.lname} , {$index.email} </option>
{/foreach}
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
