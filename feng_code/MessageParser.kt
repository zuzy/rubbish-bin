package com.lib.tcp.utils

import android.util.Log
import com.google.gson.JsonParser
import com.lib.tcp.utils.EncodeUtils.Companion.stringToHex
import com.sdk.hopeplayer.mina.beans.TCPMessage
import com.sdk.hopeplayer.mina.beans.UnicodeMsg
import com.sdk.hopeplayer.mina.consts.MessageConst
import com.sdk.hopeplayer.utils.ByteUtils




/**
 * 作者：kelingqiu on 17/11/6 10:48
 * 邮箱：42747487@qq.com
 */
class MessageParser private constructor() {

    companion object {
        private val TAG = "EncodeError"
        /**
         * 将接收到的消息进行解码并转化为TCPMessage对象
         * 1、检查消息指令；
         * 2、转义，验证校验码；
         * 3、消息解析；

         * @param bs 从客户端接收到的byte[]数组
         * *
         * @return TCPMessage 消息通信对象
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun decode(bs: ByteArray): TCPMessage? {
            var message: TCPMessage? = null
            try {
                if (checkMessage(bs))
                // 检查消息指令
                {
                    message = TCPMessage()
                    val bsSize = bs.size
                    var headerBodyAndCode = ByteUtils.bytesCopy4Index(bs, 1, bsSize - 2)
                    headerBodyAndCode = dscape(headerBodyAndCode)// 转义，验证校验码
                    message.setStartTag(bs[0])// 1、开始标识位
//                    MessageParser.setMessageAttribute(message, bs)// 设置消息属性
                    val hexHeaderBodyAndCode = ByteUtils.bytesToHexString(headerBodyAndCode)
                    val hexMessageId = hexHeaderBodyAndCode.substring(0, 4)
                    message.setMessageId(Integer.parseInt(hexMessageId, MessageConst.MESSAGE_RADIX))// 2、消息id
                    val hexCata = hexHeaderBodyAndCode.substring(4, 6)
                    message.setMessageCata(hexCata)
                    val hexLength = hexHeaderBodyAndCode.substring(6, 10)
                    message.setMessageLength(Integer.parseInt(hexLength, MessageConst.MESSAGE_RADIX))// 消息体长度
                    val deviceId = hexHeaderBodyAndCode.substring(10, 30)
                    message.setDeviceId(ByteUtils.decodePrimary(deviceId))// 客户端编号
                    var bodyStart = 30
//                    if (null != message.getPackageTag() && message.getPackageTag() == true)
//                    // 消息分包解码
//                    {
//                        bodyStart = 40
//                        val packageNum = hexHeaderBodyAndCode.substring(32, 36)
//                        val packageOrder = hexHeaderBodyAndCode.substring(36, 40)
//                        message.setPackageNum(Integer.parseInt(packageNum, MessageConst.MESSAGE_RADIX))
//                        message.setPackageOrder(Integer.parseInt(packageOrder, MessageConst.MESSAGE_RADIX))
//                        message.setMessagePackage(packageNum + packageOrder)
//                    }
                    val headerBodyAndBodyLength = hexHeaderBodyAndCode.length
                    val messageBody = hexHeaderBodyAndCode.substring(bodyStart,
                            headerBodyAndBodyLength - 2)
                    message.setMessageBody(messageBody)
                    // 解密处理
//                    val messageEncrypt = message.getMessageEncrypt()!!.trim { it <= ' ' }
//                    if (messageEncrypt == MessageConst.MESSAGE_ENCRYPT) {
//                        val encString = EncodeUtils.hexToString(messageBody)
//                        message.setMessageBody(EncodeUtils.stringToHex(EncryptUtils.base64Decode(encString)))// 将消息体转换为明文的十六进制字符串
//                    }
                    message.setMessageCode(headerBodyAndCode[headerBodyAndCode.size - 1])
                    message.setOverTag(bs[bsSize - 1])// 结束标识位
//                    MessageParser.tcpMessageDebug(message)
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }

            return message
        }

        /**
         * 调试，输出解码后的TCPMessage对象

         * @param message TCPMessage
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun tcpMessageDebug(message: TCPMessage) {
            Log.d(TAG, ("开始标识位->" + ByteUtils.byteToHexString(message.getStartTag()!!)))
            Log.d(TAG, ("消息编号->" + String.format("%04x", message.getMessageId()).toUpperCase()))
            Log.d(TAG, ("消息类型->" + message.getMessageCata()!!))
//            Log.d(TAG, ("分包标志->" + message.getPackageTag()!!))
//            Log.d(TAG, ("加密标志->" + message.getMessageEncrypt()!!))
            Log.d(TAG, ("消息长度->" + String.format("%04x", message.getMessageLength()).toUpperCase()))
            Log.d(TAG, ("终端编号->" + message.getDeviceId()!!))
//            if (null != message.getPackageTag() && message.getPackageTag() == true) {
//                Log.d(TAG, ("分包数量->" + String.format("%04x", message.getPackageNum()).toUpperCase()))
//                Log.d(TAG, ("分包序号->" + String.format("%04x", message.getPackageOrder()).toUpperCase()))
//                Log.d(TAG, ("分包信息->" + message.getMessagePackage()!!))
//            }
            Log.d(TAG, ("消息体->" + message.getMessageBody()!!))
            Log.d(TAG, ("消息校验码->" + String.format("%02x", message.getMessageCode()).toUpperCase()))
            Log.d(TAG, ("结束标识位->" + ByteUtils.byteToHexString(message.getOverTag()!!)))
        }

        /**
         * 将MessageBody转化为实体类
         *
         * @param messageBody
         */
        fun decodeBody(messageBody: String):UnicodeMsg {
            val json = JsonParser().parse(ByteUtils.hexStringToStr(messageBody)).asJsonObject
            val messageId = Integer.parseInt(json["ansId"].asString,MessageConst.MESSAGE_RADIX)
            val result = json["result"].asInt
            return UnicodeMsg(messageId,result)
        }

        /**
         * 将TCPMessage消息对象转化为十六进制字符串返回

         * @param message TCPMessage
         * *
         * @return String 十六进制字符串
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun toHexString(message: TCPMessage?): String {
            if (null == message) {
                throw NullPointerException()
            }
            val stringBuffer = StringBuffer()
            stringBuffer.append(ByteUtils.byteToHexString(message.getStartTag()!!))
            stringBuffer.append(String.format(MessageConst.FOUR_HEX_FORMAT, message.getMessageId()).toUpperCase())
            stringBuffer.append(message.getMessageCata())
            stringBuffer.append(String.format(MessageConst.FOUR_HEX_FORMAT, message.getMessageLength()).toUpperCase())
            stringBuffer.append(ByteUtils.encodePrimary(message.getDeviceId()))
//            if (null != message.getPackageTag() && message.getPackageTag() == true) {
//                stringBuffer.append(String.format(MessageConst.FOUR_HEX_FORMAT, message.getPackageNum()).toUpperCase())
//                stringBuffer.append(String.format(MessageConst.FOUR_HEX_FORMAT, message.getPackageOrder()).toUpperCase())
//            }
            stringBuffer.append(message.getMessageBody())
            stringBuffer.append(ByteUtils.byteToHexString(message.getMessageCode()!!))
            stringBuffer.append(ByteUtils.byteToHexString(message.getOverTag()!!))
            return stringBuffer.toString()
        }

//        /**
//         * 解析消息体属性，保留位，分包标识，加密方式，消息体长度，消息体属性
//
//         * @param message TCPMessage
//         * *
//         * @param bs      收到的消息字节数组
//         */
//        fun setMessageAttribute(message: TCPMessage?, bs: ByteArray) {
//            val attribute = ByteUtils.bytesCopy4Index(bs, 3, 4)
//            val attributeHexString = ByteUtils.bytesToHexString(attribute)
//            if (null == message || attributeHexString.isEmpty()) {
//                throw NullPointerException()
//            }
//            // 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//            var binaryString = Integer.toBinaryString(Integer.parseInt(attributeHexString, MessageConst.MESSAGE_RADIX))
//            binaryString = String.format("%1$0" + (16 - binaryString.length) + "d", 0) + binaryString// 补位操作，1byte=8bit
//            // 属性保留位，15 14
//            val messageRetain = binaryString.substring(0, 2)
//            message.setMessageCata(messageRetain)
//            // 分包标识，13
////            val packageTag = binaryString.substring(2, 3)
////            message.setPackageTag(Integer.parseInt(packageTag) == 1)
//            // 数据加密方式，12，当第12位为1，表示消息体经过DES 算法加密
////            val encrypt = binaryString.substring(3, 4)
////            message.setMessageEncrypt(encrypt)
//            // 消息体长度，13 12 11 10 9 8 7 6 5 4 3 2 1 0
//            val mssageLength = binaryString.substring(2, MessageConst.ATTRIBUTE_LEN)
//            message.setMessageLength(Integer.parseInt(mssageLength, 2))
//            // 消息体属性
//            message.setMessageAttribute(Integer.parseInt(attributeHexString, MessageConst.MESSAGE_RADIX))
//        }

        /**
         * 消息指令转义还原，接收消息时：接收->转义还原——>验证校验码——>解析消息，转义规则如下：
         * 采用0x7e 表示，若校验码、消息头以及消息体中出现0x7e，则要进行转义处理：
         * 0x7e<——>0x7d 后紧跟一个0x02；
         * 0x7d<——>0x7d 后紧跟一个0x01。
         * 1、消息转义还原；
         * 2、检查校验码（校验码指从消息头开始，同后一字节异或，直到校验码前一个字节，占用一个字节）；

         * @param headerBodyAndCode 消息头、消息体、校验码
         * *
         * @return byte[] 转义还原后的消息头、消息体、校验码字节数组
         */
        private fun dscape(headerBodyAndCode: ByteArray?): ByteArray {
            if (null == headerBodyAndCode || headerBodyAndCode.size < MessageConst.MESSAGE_HBCODE) {

                throw IllegalStateException()
            }
            // 转义还原
            val dscapeArray = unpacker(headerBodyAndCode)
            if (dscapeArray != null && dscapeArray.isEmpty()) {
                throw NullPointerException()
            }
            // 验证校验码
            val arrayLength = dscapeArray!!.size
            if (arrayLength < 2) {
                throw IndexOutOfBoundsException()
            }
            val headerAndBody = ByteUtils.bytesCopy4Index(dscapeArray, 0, arrayLength - 2)// 消息头和消息体
            val messageXor = ByteUtils.bytesXor(headerAndBody)// 计算校验码
            val messageCode = dscapeArray[dscapeArray.size - 1]// 消息指令中的校验码字节
            if (messageXor != messageCode) {
                throw IllegalStateException()
            }
            return dscapeArray
        }

        /**
         * 转义还原
         * 0x7e<——>0x7d 后紧跟一个0x02；
         * 0x7d<——>0x7d 后紧跟一个0x01。

         * @param headerBodyAndCode 消息头、消息体、校验码字节数组
         * *
         * @return byte[] 转义后的消息头、消息体、校验码字节数组
         */
        private fun unpacker(headerBodyAndCode: ByteArray): ByteArray? {
            var dscapeArray: ByteArray? = null
            val bsSize = headerBodyAndCode.size
            // 转义还原
            val list = ArrayList<Byte>()
            var i = 0
            while (i < bsSize) {
                val byte1 = headerBodyAndCode[i]
                if (byte1 == MessageConst.CONVERT_FLAG)
                // 0x7d
                {
                    if (i + 1 < bsSize) {
                        val byte2 = headerBodyAndCode[i + 1]
                        if (byte2 == MessageConst.CONVERT_SUFFIX)
                        // 0x02
                        {
                            list.add(MessageConst.MESSAGE_FLAG)
                            i += 1
                        } else if (byte2 == MessageConst.CONVERT_PREFIX)
                        // 0x01
                        {
                            list.add(MessageConst.CONVERT_FLAG)
                            i += 1
                        }
                    } else {
                        list.add(byte1)
                    }
                } else {
                    list.add(byte1)
                }
                i++
            }
            if (list.size != 0) {
                val listSize = list.size
                dscapeArray = ByteArray(listSize)
                for (x in 0..listSize - 1) {
                    dscapeArray[x] = list[x]
                }
            }
            Log.d(TAG, ("headerBodyAndCode：" + ByteUtils.bytesToHexString(headerBodyAndCode)))
            Log.d(TAG, ("headerBodyAndCode->dscape：" + ByteUtils.bytesToHexString(dscapeArray)))
            return dscapeArray
        }

        /**
         * 消息指令合法性检查
         * 1、指令字节数组长度不能小于最小长度（标识位[1]+消息头[15/19]+校验码[1]+标识位[1]）；
         * 2、指令指节数组包含开始标识位及结束标识位；

         * @param bs 收到的字节数组
         * *
         * @return boolean true：合法，false：不合法
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun checkMessage(bs: ByteArray?): Boolean {
            if (null == bs || bs.size < MessageConst.MESSAGE_MINLEN) {
                throw IllegalStateException()
            }
            val bsSize = bs.size
            val startTag = bs[0] // 开始标识位
            val overTag = bs[bsSize - 1] // 结束标识位
            if (startTag != MessageConst.MESSAGE_FLAG || overTag != MessageConst.MESSAGE_FLAG) {
                val hexString = ByteUtils.bytesToHexString(bs)
                Log.d(TAG, (hexString))
                throw IllegalStateException()
            }
            return true
        }

        fun playStatusHexStr(messageId: Int, args: Int): String {
            val stringBuffer = StringBuffer()
            stringBuffer.append(String.format(MessageConst.FOUR_HEX_FORMAT, messageId).toUpperCase())
            stringBuffer.append(String.format(MessageConst.EIGHT_HEX_FORMAT, args).toUpperCase())
            return stringBuffer.toString()
        }

        fun decodePlayControlId(messageBody: String) : Int{
            return Integer.parseInt(messageBody.substring(0,4), MessageConst.MESSAGE_RADIX)
        }

        fun decodePlayControlArgs(messageBody: String) : Int{
            return Integer.parseInt(messageBody.substring(4,messageBody.length), MessageConst.MESSAGE_RADIX)
        }



        fun createPlayListHexStr(id: Int, name: String): String {
            val stringBuffer = StringBuffer()
            stringBuffer.append(String.format(MessageConst.FOUR_HEX_FORMAT, id).toUpperCase())
            stringBuffer.append(stringToHex(name).toUpperCase())
            return stringBuffer.toString()
        }
    }
}