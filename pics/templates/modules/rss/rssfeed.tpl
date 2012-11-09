<!-- start rssfeed.tpl -->
<table class="fiftyperc">
<tbody>
{$rssTitle}
{foreach item=index from=$rss}
<tr class="left">
<td class="leftB"> <a href="{$index.link}" target="new">{$index.title}</a> </td>
</tr>
{/foreach}
</tbody>
</table>
<!-- end rssfeed.tpl -->
