<!-- cart start -->
<table id="receipt" cellspacing="0" cellpadding="0">
<tbody>
{* index[1] ==> productQuantity & index[2] ==> productName & index[3] ==> productTotal *}
{foreach name=outer item=index from=$basketItems}
<tr>
	<td id="receipt">	{$index[1]}x				</td>
	<td id="prname">	{$index[2]}					</td>
	<td id="receipt">	{$index[3]|currency:$crcy}	</td>		
</tr>
{/foreach}
{if $subtotal}
	<tr>
		<td id="receipt" colspan="2"> subtotal		</td>
		<td id="receipt"> {$subtotal|currency:$crcy}</td>
	</tr>
{else}
	<tr>
        <td> Cart is empty</td>
    </tr>
{/if}
</tbody>
</table>
<table id="actbutton" cellspacing="0" cellpadding="0">
<tbody>
<tr>
<td id="actbutton"> <a id="actbutton" href="checkout.php"> CheckOut </a> </td>
</tr>
</tbody>
</table>
<!-- cart end -->
