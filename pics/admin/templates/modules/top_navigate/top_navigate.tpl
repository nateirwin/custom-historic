<!-- top_navigate.tpl -->

{if $ctSize > 0}
<table id="brdr">
<tr>
	<td> Category:
	{foreach item=index from=$topNavigate}
	{ if $index.parentId == 0 } 
		<a class="linkbutton" href="add.php?cid={$index.categId}"> {$index.name} </a>
	{else}
		>>
		<a class="linkbutton" href="add.php?cid={$index.categId}"> {$index.name} </a>
	{/if} 
	{/foreach}
	</td>
</tr>
</table>
{/if}

{if $snSize > 0}
<table id="brdr">
<tr>
	<td> SubCategories:
	{foreach item=index from=$subNavigate}
		<a class="linkbutton" href="add.php?cid={$index.categId}"> {$index.name} </a>
	{foreachelse}
	{/foreach}
	</td>
</tr>
</table>
{/if}
<!-- top_navigate.tpl end -->

