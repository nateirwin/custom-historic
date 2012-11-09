{if $firstPage == "true" }
	<form name="chooseuser" action={$action} method="POST">
	<table class="forms">
	<tr>
	    <td class="formch">  </td>
	    <td class="formc"> Name   </td>
	    <td class="formc"> Email/UserName  </td>
	    <td class="formc"> userId  </td>
	</tr>
	{foreach item=index from=$users}
	<tr>
	    <td class="formch"><input type="radio" name="uid" value="{$index.userId}"></td>
	    <td class="formc"> {$index.lname}, {$index.fname} {$index.mname} </td>
	    <td class="formc"> {$index.email}                       </td>
	    <td class="formc"> {$index.userId}                      </td>
	</tr>
	{/foreach}
	</table>
                                                                                                                                                                    
	<table class="buttons">
	<tr>
	<td class="buttons">
	<a class="button" href="javascript:document.chooseuser.submit(); " title="Insert pics for selected user"> Submit </a>
	</td>
	</tr>
	</table>
	</form>
{else}
	{if $success == ""}
	<form name="pics" method="POST" action={$action} enctype="multipart/form-data">
	<input type="hidden" name="uid" value="{$uid}">
	<table class="buttons">
	<tr>
	<td class="buttons">
		<input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	<td class="buttons">
	    <input name="userfile[]" type="file">
	</td>
	</tr>
	<tr>
	<td class="buttons" colspan="2">
		<!-- <input type="submit" value="Upload Files"> -->
		<a class="button" href="javascript:document.pics.submit(); " title="Insert pics for selected user"> Submit </a>
	</td>
	</tr>
	</table>
	</form>
	{/if}
{/if}
