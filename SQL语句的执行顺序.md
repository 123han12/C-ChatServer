## 1、`SQL`语句完整的执行顺序

`SQL`语言不同于其他编程语言的最明显特征是处理代码的顺序。 在大多[数据库语言](https://so.csdn.net/so/search?q=%E6%95%B0%E6%8D%AE%E5%BA%93%E8%AF%AD%E8%A8%80&spm=1001.2101.3001.7020)中，代码按编码顺序被处理。但在 `SQL` 语句中，第一个被处理的子句式 `FROM`，而不是第一出现的 `SELECT`。

-   `FROM` 子句组装来自不同数据源的数据；
-   `WHERE` 子句基于指定的条件对记录行进行筛选；
-   `GROUP BY` 子句将数据划分为多个分组；
-   使用聚集函数进行计算；
-   使用 `HAVING` 子句筛选分组；
-   计算所有的表达式；
-   `SELECT` 的字段；
-   使用 `ORDER BY` 对结果集进行排序。

![在这里插入图片描述](https://img-blog.csdnimg.cn/453b45f7091c433a81a5113e765630c0.png)

## 2、`SQL` 查询处理的步骤序号

下面**每个步骤都会产生一个虚拟表**，该虚拟表被用作下一个步骤的输入。这些虚拟表对调用者（客户端应用程序或者外部查询）不可用。**只有最后一步生成的表才会会给调用者**。如果没有在查询中指定某一个子句， 将跳过相应的步骤。

-   `FROM <left_table>`
-   `<join_type> JOIN <right_table>`
-   `ON <join_condition>`
-   `WHERE <where_condition>`
-   `GROUP BY <group_by_list>`
-   `WITH {CUBE | ROLLUP}`
-   `HAVING <having_condition>`
-   `SELECT`
-   `DISTINCT`
-   `ORDER BY <order_by_list>`
-   `<TOP_specification> <select_list>`

## 3、逻辑查询处理阶段简介

-   **`FROM`** **：** 对 `FROM` 子句中的前两个表执行笛卡尔积(交叉联接)，生成虚拟表 `VT1`。
-   **`ON`** **：** 对 `VT1` 应用 `ON` 筛选器，只有那些使为真才被插入到 `TV2`。
-   **`OUTER (JOIN)`** **：** 如果指定了 `OUTER JOIN`（相对于 `CROSS JOIN` 或 `INNER JOIN`），保留表中未找到匹配的行将作为外部行添加到 `VT2`，生成 `TV3`。如果 `FROM` 子句包含两个以上的表，则对上一个联接生成的 结果表和下一个表重复执行步骤 1 到步骤 3，直到处理完所有的表位置。
-   **`WHERE`** **：** 对 `TV3` 应用 `WHERE` 筛选器，只有使为 `true` 的行才插入 `TV4`。
-   **`GROUP BY`** **：** 按 `GROUP BY` 子句中的列列表对 `TV4` 中的行进行分组，生成 `TV5`。
-   **`CUTE|ROLLUP`** **：** 把超组插入 `VT5`，生成 `VT6`。
-   **`HAVING`** **：** 对 `VT6` 应用 `HAVING` 筛选器，只有使为 `true` 的组插入到 `VT7`。
-   **`SELECT`** **：** 处理 `SELECT` 列表，产生`VT8`。
-   **`DISTINCT`** **：** 将重复的行从 `VT8` 中删除，产品 `VT9`。
-   **`ORDER BY`** **：** 将 `VT9` 中的行按 `ORDER BY` 子句中的列列表顺序，生成一个游标（`VC10`）。
-   **`TOP`** **：** 从 `VC10` 的开始处选择指定数量或比例的行，生成表 `TV11`，并返回给调用者。 `WHERE`子句中的条件书写顺序。