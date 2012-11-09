<script language="JavaScript" SRC="templates/js/popup.js">
</script>
<form name="deletepics" method="POST">
<table class="buttons">
{foreach item=index from=$userPics}
<tr>
<td>
<div class="pics">
<input type="checkbox" name="{$index.pic}" value="{$index.pic}"> <a href="./images/pop_{$index.pic}" onclick="newWindow(this.href,'this.name','yes'); return false;" title="Enlarge Picture"> <img alt="thumbnail" src="./images/thb_{$index.pic}"> </a>
</div>
<div class="admincomments">
{$index.adminComment}
</div>
<div class="usercomments">
{$index.userComment}
</div>
<div class="clear">
</div>
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
<a class="button" href="javascript:document.deletepics.submit(); " title="Click here to Delete Selected Pictures"> Submit </a>
</td>
</tr>
</table>
{/if}
</form>

