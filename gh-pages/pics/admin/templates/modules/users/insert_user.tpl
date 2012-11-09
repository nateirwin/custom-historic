<form name="insert" method="POST">
<table class="forms">
<tr>
<td class="forml"> Title:
</td>
<td class="formr"> 
{if $inFlag eq "true" }
{$inserted.title}
{else}
<input class="txtbox" type="text" name="title" value="{$title}" size="40" maxlength="35" >
{/if}
</td>
</tr>
<tr>
<td class="forml"> *First Name:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.fname}
{else}
<input class="txtbox" type="text" name="fname" value="{$fname}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> MI:
</td>
<td class="formr"> 
{if $inFlag eq "true" }
{$inserted.mname}
{else}
<input class="txtbox" type="text" name="mname" value="{$mname}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> *Last Name:
</td>
<td class="formr"> 
{if $inFlag eq "true" }
{$inserted.lname}
{else}
<input class="txtbox" type="text" name="lname" value="{$lname}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> Address:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.address}
{else}
<input class="txtbox" type="text" name="address" value="{$address}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> City:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.city}
{else}
<input class="txtbox" type="text" name="city" value="{$city}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> State:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.state}
{else}
<input class="txtbox" type="text" name="state" value="{$state}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> Zip:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.zip}
{else}
<input class="txtbox" type="text" name="zip" value="{$zip}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> Home Tel:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.homeTel}
{else}
<input class="txtbox" type="text" name="htel" value="{$htel}" size="40" maxlength="35">
{/if}
</td>
</tr>
<td class="forml"> Work Tel:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.workTel}
{else}
<input class="txtbox" type="text" name="wtel" value="{$wtel}" size="40" maxlength="35">
{/if}
</td>
</tr>
<td class="forml"> Cell Tel:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.cellTel}
{else}
<input class="txtbox" type="text" name="ctel" value="{$ctel}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> Contact Person:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.contactPerson}
{else}
<input class="txtbox" type="text" name="ct" value="{$ct}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> Contact Tel:
</td>
<td class="formr">
{if $inFlag eq "true" }
{$inserted.contactTel}
{else}
<input class="txtbox" type="text" name="ctt" value="{$ctt}" size="40" maxlength="35">
{/if}
</td>
</tr>
<tr>
<td class="forml"> *Password:</td>
<td class="formr">
{if $inFlag eq "true" }
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
{if $inFlag eq "true" }
{$inserted.email}
{else}
<input class="txtbox" type="text" name="email" value="{$email}" size="40" maxlength="35">
{/if}
</td>
</tr>
{if $inFlag eq "true" }
{else}
<tr>
<td class="forml"> * Email/Unique User Id:
</td>
<td class="formr">
<input class="txtbox" type="text" name="verify" value="{$verify}" size="40" maxlength="35">
</td>
</tr>
{/if}
</table>

{if $inFlag eq "true" }
{else}
<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.insert.submit(); " title="Click here to Insert New User"> Submit </a>
</td>
</tr>
</table>
{/if}
</form>
