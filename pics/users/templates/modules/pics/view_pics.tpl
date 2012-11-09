<script language="JavaScript" SRC="templates/js/popup.js">
</script>
<table class="buttons">
{foreach item=index from=$userPics}
<tr>
<td>
<div class="pics">
<a href="./images/pop_{$index.pic}" onclick="newWindow(this.href,'this.name','yes'); return false;" title="Enlarge Picture"> <img alt="thumbnail" src="./images/thb_{$index.pic}"> </a>
<div class="adminComments">
<a href="./images/{$index.pic}" onclick="newWindow(this.href,'this.name','yes'); return false;" title="View Full Size of the Picture"> [Full Size] </a>
</div>
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
