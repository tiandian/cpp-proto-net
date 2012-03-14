using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ProtoBuf;

namespace QuickArbitrage.Common.Contract
{
    [ProtoContract]
    public class Quote
    {
        // 合约代码
        [ProtoMember(1)]
        public string symbol { get; set; }

        // 交易日
        [ProtoMember(2)]
        public string trading_day { get; set; }

        // 交易所代码
        [ProtoMember(3)]
        public string exchange_id { get; set; }

        // 合约在交易所的代码
        [ProtoMember(4)]
        public string exchange_symbol_id { get; set; }

        // 最新价
        [ProtoMember(5)]
        public double last { get; set; }

        // 上次结算价
        [ProtoMember(6)]
        public double prev_settlement_price { get; set; }

        // 昨收盘
        [ProtoMember(7)]
        public double prev_close { get; set; }

        // 昨持仓量
        [ProtoMember(8)]
        public double prev_open_interest { get; set; }

        // 今开盘
        [ProtoMember(9)]
        public double open { get; set; }

        // 最高价
        [ProtoMember(10)]
        public double high { get; set; }

        // 最低价
        [ProtoMember(11)]
        public double low { get; set; }

        // 数量
        [ProtoMember(12)]
        public int volume { get; set; }

        // 成交金额
        [ProtoMember(13)]
        public double turnover { get; set; }

        // 持仓量
        [ProtoMember(14)]
        public double open_interest { get; set; }

        // 今收盘
        [ProtoMember(15)]
        public double close { get; set; }

        // 本次结算价
        [ProtoMember(16)]
        public double settlement_price { get; set; }

        // 涨停板价
        [ProtoMember(17)]
        public double upper_limit_price { get; set; }

        // 跌停板价
        [ProtoMember(18)]
        public double lower_limit_price { get; set; }

        // 昨虚实度
        [ProtoMember(19)]
        public double prev_delta { get; set; }

        // 今虚实度
        [ProtoMember(20)]
        public double curr_delta { get; set; }

        // 最后修改时间
        [ProtoMember(21)]
        public string update_time { get; set; }

        // 最后修改毫秒
        [ProtoMember(22)]
        public int update_millisec { get; set; }

        // 申买价一
        [ProtoMember(23)]
        public double bid { get; set; }

        // 申买量一
        [ProtoMember(24)]
        public int bid_size { get; set; }

        // 申卖价一
        [ProtoMember(25)]
        public double ask { get; set; }

        // 申卖量一
        [ProtoMember(26)]
        public int ask_size { get; set; }

        // 申买价二
        [ProtoMember(27)]
        public double bid_2 { get; set; }

        // 申买量二
        [ProtoMember(28)]
        public int bid_size_2 { get; set; }

        // 申卖价二
        [ProtoMember(29)]
        public double ask_2 { get; set; }

        // 申卖量二
        [ProtoMember(30)]
        public int ask_size_2 { get; set; }

        // 申买价三
        [ProtoMember(31)]
        public double bid_3 { get; set; }

        // 申买量三
        [ProtoMember(32)]
        public int bid_size_3 { get; set; }

        // 申卖价三
        [ProtoMember(33)]
        public double ask_3 { get; set; }

        // 申卖量三
        [ProtoMember(34)]
        public int ask_size_3 { get; set; }

        // 申买价四
        [ProtoMember(35)]
        public double bid_4 { get; set; }

        // 申买量四
        [ProtoMember(36)]
        public int bid_size_4 { get; set; }

        // 申卖价四
        [ProtoMember(37)]
        public double ask_4 { get; set; }

        // 申卖量四
        [ProtoMember(38)]
        public int ask_size_4 { get; set; }

        // 申买价五
        [ProtoMember(39)]
        public double bid_5 { get; set; }

        // 申买量五
        [ProtoMember(40)]
        public int bid_size_5 { get; set; }

        // 申卖价五
        [ProtoMember(41)]
        public double ask_5 { get; set; }

        // 申卖量五
        [ProtoMember(42)]
        public int ask_size_5 { get; set; }

        // 当日均价
        [ProtoMember(43)]
        public double average_price { get; set; }
    }
}
