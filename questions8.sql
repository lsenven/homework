select RegionDescription
, FirstName
, LastName
, birthday
from
(
  select RegionId as r, max(Employee.Birthdate) as birthday
  from Employee
    join EmployeeTerritory on Employee.Id = EmployeeTerritory.EmployeeId
    join Territory on TerritoryId = Territory.Id
  group by RegionId
)
join 
(
    select FirstName
    , LastName
    , Birthdate
    , RegionId
    , EmployeeId
    from Employee
    join EmployeeTerritory on Employee.Id = EmployeeTerritory.EmployeeId
    join Territory on TerritoryId = Territory.Id
)on Birthdate = birthday  
join Region on Region.Id = RegionId
group by EmployeeId
order by r;