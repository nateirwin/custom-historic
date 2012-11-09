<form name="addcomments" method="POST">
<table class="buttons">
{foreach item=index from=$userPics}
<tr>
<td class="pics">
<a href="./images/pop_{$index.pic}" onclick="newWindow(this.href,'this.name','yes'); return false;" title="Enlarge Picture"> <img alt="thumbnail" src="./images/thb_{$index.pic}"> </a>
</td>
<td class="pics">
<textarea name="{$index.pic}" cols="40" rows="3">{$index.userComment}</textarea> 
</td>
</tr>
{foreachelse}
{if $uid >= 1 }
<tr>
<td>
No pictures available for the selected User
</td>
</tr>
{else}
<tr>
<td>
Select a user and click on the submit button
</td>
</tr>
{/if}
{/foreach}
</table>

{if $userPics != null}
<table class="buttons">
<tr>
<td class="buttons">
<a class="button" href="javascript:document.addcomments.submit(); " title="Click here to add comments to the Selected Pictures"> Submit </a>
</td>
</tr>
</table>
{/if}
</form>
