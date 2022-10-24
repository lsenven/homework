select CompanyName, round(delayPacke * 100.0 / sum, 2) as percent
from
    (
      select ShipVia,count(*) as sum
      from 'Order'
      group by ShipVia
    )as sumCnt
join 
    (
    select ShipVia, count(*) as delayPacke
    from'Order'
    where ShippedDate > RequiredDate 
    group by ShipVia
    ) as delayPacke
    on sumCnt.ShipVia = delayPacke.ShipVia
join Shipper on sumCnt.ShipVia = Shipper.Id
order by percent DESC;