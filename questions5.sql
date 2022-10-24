select ProductName
, CompanyName
, ContactName
from (
      select ProductName
      , min(OrderDate)
      , CompanyName
      , ContactName
      from (
            select Id as prodouctid , ProductName 
            from Product 
            where Discontinued != 0
           ) as discontinued
      join OrderDetail on ProductId = prodouctid
      join 'Order' on 'Order'.Id = OrderDetail.OrderId
      join Customer on CustomerId = Customer.Id
      group by prodouctid
    )
order by ProductName;