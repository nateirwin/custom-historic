<form name="deleteuser" method="POST">
<table class="forms">
<tr>
	<td class="formch">  </td>
    <td class="formc"> Name   </td>
    <td class="formc"> Email/UserName  </td>
</tr>
{foreach item=index from=$users}
<tr>
	<td class="formch"><input type="checkbox" name="{$index.userId}" value="{$index.userId}"></td>
    <td class="formc"> {$index.fname} {$index.mname} {$index.lname} </td>
    <td class="formc"> {$index.email}                       </td>
</tr>
{/foreach}
</table>

<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.deleteuser.submit(); " title="Click here to Delete New User"> Submit </a>
</td>
</tr>
</table>
</form>
