select 
	ifnull(Customer.CompanyName, "MISSING_NAME")
    ,list.CustomerId
    ,round(list.price, 2)
from
	(
        select
            'Order'.CustomerId, 
            sum(OrderDetail.Quantity * OrderDetail.UnitPrice) price,
            ntile(4) over 
            (
                order by sum(OrderDetail.Quantity * OrderDetail.UnitPrice) 
            ) nprice
        from
            'Order'join OrderDetail on 'order'.Id = OrderDetail.OrderId
        group by 'Order'.CustomerId
    ) list
    left join Customer on Customer.Id = list.customerId
where list.nprice = 1;