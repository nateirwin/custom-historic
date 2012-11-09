{if $firstPage == "true" }
	<table class="forms">
	<tr>
	    <td class="formc"> Name   </td>
	    <td class="formc"> Email/UserName  </td>
	</tr>
	{foreach item=index from=$users}
	<tr>
	    <td class="formc"> <a href="edit.php?uid={$index.userId}&db=true" title="Click here to edit user account" > {$index.fname} {$index.mname} {$index.lname} </a> </td>
	    <td class="formc"> {$index.email}                       </td>
	</tr>
	{/foreach}
	</table>
{else}
	<form name="edituser" action={$action} method="POST">
	<table class="forms">
	<tr>
	<td class="forml"> Title:
	</td>
	<td class="formr"> 
		{if $success != "" }
		{$updated.title}
		{else}
		<input class="txtbox" type="text" name="title" value="{$title}" size="40" maxlength="35" >
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> *First Name:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.fname}
		{else}
		<input class="txtbox" type="text" name="fname" value="{$fname}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> MI:
	</td>
	<td class="formr"> 
		{if $success != "" }	
		{$updated.mname}
		{else}
		<input class="txtbox" type="text" name="mname" value="{$mname}" size="40" maxlength="35">
	{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> *Last Name:
	</td>
	<td class="formr"> 
		{if $success != "" }
		{$updated.lname}
		{else}
		<input class="txtbox" type="text" name="lname" value="{$lname}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> Address:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.address}
		{else}
		<input class="txtbox" type="text" name="address" value="{$address}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> City:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.city}
		{else}
		<input class="txtbox" type="text" name="city" value="{$city}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> State:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.state}
		{else}
		<input class="txtbox" type="text" name="state" value="{$state}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> Zip:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.zip}
		{else}
		<input class="txtbox" type="text" name="zip" value="{$zip}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> Home Tel:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.homeTel}
		{else}
		<input class="txtbox" type="text" name="htel" value="{$htel}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<td class="forml"> Work Tel:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.workTel}
		{else}
		<input class="txtbox" type="text" name="wtel" value="{$wtel}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<td class="forml"> Cell Tel:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.cellTel}
		{else}
		<input class="txtbox" type="text" name="ctel" value="{$ctel}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> Contact Person:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.contactPerson}
		{else}
		<input class="txtbox" type="text" name="ct" value="{$ct}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> Contact Tel:
	</td>
	<td class="formr">
		{if $success != "" }
		{$updated.contactTel}
		{else}
		<input class="txtbox" type="text" name="ctt" value="{$ctt}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> *Password:
	</td>
	<td class="formr">
		{if $success != "" }
		{$userPass}
		{else}
		<input class="txtbox" type="text" name="passwd" value="{$passwd}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
	<tr>
	<td class="forml"> * Email/Unique User Id:
	</td>
	<td class="formr">
		{if $success != ""}
		{$updated.email}
		{else}
		<input class="txtbox" type="text" name="email" value="{$email}" size="40" maxlength="35">
		{/if}
	</td>
	</tr>
</table>

	{if $success != "" }
	{else}
	<table class="buttons">
	<tr>
	<td class="buttons">
	<a class="button" href="javascript:document.edituser.submit(); " title="Click here to Edit the User Information"> Submit </a>
	</td>
	</tr>
	</table>
	{/if}
</form>
{/if}
