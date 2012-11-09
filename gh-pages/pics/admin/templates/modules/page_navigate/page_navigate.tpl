<!-- start totalPages.tpl -->
<table cellspacing="5" id="center">
<tbody>
<tr class="left">
    <td class="centG">
		{assign var="i" value="0"}
		{section name=pgs loop=$totalPages step=1}
		<a href="orders.php?pg={$i++}"> [{$i}] </a>
		{/section}
	</td>
</tr>
</tbody>
</table>
<!-- end totalPages.tpl -->

