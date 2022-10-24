select
    Id
    , OrderDate
    , PrevOrderDate
    , ROUND(julianday(OrderDate) - julianday(PrevOrderDate), 2)
from (
    select Id
            , OrderDate
            , LAG(OrderDate, 1, OrderDate) over (order by OrderDate ) as PrevOrderDate
    from 'Order' 
    where CustomerId = 'BLONP'
    order by OrderDate 
    limit 10
);