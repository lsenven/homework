select y.CategoryName, x.Categorycount, x.avgunitprice, x.minunitprice, x.maxunitprice, x.sumunitsONOrder
from
	(
        select CategoryId
        , count(CategoryId) Categorycount
        , round(AVG(UnitPrice), 2) avgunitprice
        , min(UnitPrice) minunitprice
        , max(UnitPrice) maxunitprice
        , sum(UnitsONOrder) sumunitsONOrder
        from Product
        group by CategoryId
        having Categorycount > 10
    ) x
    join
    (
        select distinct(CategoryId), CategoryName
        from Productdetails_V
    ) y 
    on x.CategoryId = y.CategoryId
order by x.CategoryId;