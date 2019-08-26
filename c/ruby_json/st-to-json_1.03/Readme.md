
按照format打印struct;

修复内存bug;

增加 &1-xx 匹配固定长度字符串到

增加长度策略:
    当format 长度 <0 会一直匹配到keys到 NULL,
    当format 长度过长, 会匹配到keys 为NULL, 然后剩余的全部成员放入 <def_array>