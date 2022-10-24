select 
	Id, ShipCountry, 
	case ShipCountry 
		when'USA' 
			then 'NorthAmerica' 
		when 'Mexico' 
			then 'NorthAmerica' 
		when 'Canada' 
			then 'NorthAmerica' 
		else 'OtherPlace' 
        end
from 'Order'
where Id >= 15445
order by Id ASC
limit 20;