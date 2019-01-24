package com.lib.tcp.utils

import com.sdk.hopeplayer.Constants
import com.sdk.hopeplayer.mina.beans.TCPMessage
import com.sdk.hopeplayer.mina.beans.TCPMsgType
import com.sdk.hopeplayer.mina.consts.MessageConst
import com.sdk.hopeplayer.utils.ByteUtils
import java.nio.charset.Charset


/**
 * 作者：kelingqiu on 17/11/6 10:29
 * 邮箱：42747487@qq.com
 */
class MessageBuilder private constructor() {
    companion object {
        // 消息流水号
        private var MESSAGE_SERIAL: Int = 0x00

        /**
         * 将消息对象编码为字节数组

         * @param message TCPMessage
         * *
         * @return byte[] 编码后的字节数组
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun encode(message: TCPMessage): ByteArray {
            val messageCode = message.getMessageCode()!!
            val computeCode = getMessageCode(message)
            if (messageCode != computeCode) {
                throw IllegalStateException()
            }
            val hexTcpMessage = escape(message)
            return ByteUtils.hexStringToBytes(hexTcpMessage)
        }

        /**
         * 消息对象编码转义处理

         * @param message TCPMessage
         * *
         * @return String 消息对象转化后的十六进制字符串
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        private fun escape(message: TCPMessage?): String {
            if (null == message) {
                throw NullPointerException()
            }
            return escape(MessageParser.toHexString(message))
        }

        /**
         * 十六进制度消息字符串编码转义处理

         * @param hexMessage 十六进制度消息字符串
         * *
         * @return String 转义后的十六进制度消息字符串
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        private fun escape(hexMessage: String): String {
            if (hexMessage.isEmpty()) {
                throw NullPointerException()
            }
            val bs = ByteUtils.hexStringToBytes(hexMessage)
            return escape(bs)
        }

        /**
         * 消息字节数组转义处理

         * @param bs 消息字节数组
         * *
         * @return String 转义后的十六进制度消息字符串
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        private fun escape(bs: ByteArray?): String {
            if (null == bs || bs.size < MessageConst.MESSAGE_MINLEN) {
                throw IllegalStateException()
            }
            val bsSize = bs.size
            val stringBuffer = StringBuffer()
            stringBuffer.append(ByteUtils.byteToHexString(bs[0]))
            for (i in 1..bsSize - 1 - 1) {
                val bi = bs[i]
                if (bi == MessageConst.MESSAGE_FLAG)
                // 0x7E->0x7D 0x02
                {
                    stringBuffer.append(ByteUtils.byteToHexString(MessageConst.CONVERT_FLAG))// 0x7D
                    stringBuffer.append(ByteUtils.byteToHexString(MessageConst.CONVERT_SUFFIX))// 0x02
                } else if (bi == MessageConst.CONVERT_FLAG)
                // 0x7D->0x7D 0x01
                {
                    stringBuffer.append(ByteUtils.byteToHexString(MessageConst.CONVERT_FLAG))// 0x7D
                    stringBuffer.append(ByteUtils.byteToHexString(MessageConst.CONVERT_PREFIX))// 0x01
                } else {
                    stringBuffer.append(ByteUtils.byteToHexString(bi))
                }
            }
            stringBuffer.append(ByteUtils.byteToHexString(bs[bsSize - 1]))
            return stringBuffer.toString().toUpperCase()
        }

        /**
         * 将有效的未转义的十六进制消息字符串转化为消息对象

         * @param message 有效的十六进制消息字符串
         * *
         * @return TCPMessage TCPMessage
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun getMessage(message: String): TCPMessage {
            if (message.isEmpty()) {
                throw NullPointerException()
            }
            val tcpMessage = TCPMessage()
            val bs = ByteUtils.hexStringToBytes(message)
            if (MessageParser.checkMessage(bs)) {
                val bsSize = bs.size
                val headerAndBody = ByteUtils.bytesCopy4Index(bs, 1, bsSize - 3)// 消息头和消息体
                val messageXor = ByteUtils.bytesXor(headerAndBody)// 计算校验码
                val messageCode = bs[bsSize - 2]// 消息指令中的校验码字节
                if (messageXor != messageCode) {
                    throw IllegalStateException()
                }
                val headerBodyAndCode = message.substring(2, message.length - 2)
                tcpMessage.setStartTag(bs[0])// 1、开始标识位
                val hexMessageId = headerBodyAndCode.substring(0, 4)
                tcpMessage.setMessageId(Integer.parseInt(hexMessageId, MessageConst.MESSAGE_RADIX))// 消息编号
                val hexCata = headerBodyAndCode.substring(4, 6)
                tcpMessage.setMessageCata(hexCata)// 消息体属性
                val hexLength = headerBodyAndCode.substring(6, 10)
                tcpMessage.setMessageLength(Integer.parseInt(hexLength, MessageConst.MESSAGE_RADIX))//消息体长度
//                tcpMessage.setMessageAttribute(Integer.parseInt(hexAttribute, MessageConst.MESSAGE_RADIX))// 消息体属性
                val deviceId = ByteUtils.decodePrimary(headerBodyAndCode.substring(10, 30))
                tcpMessage.setDeviceId(deviceId)// 客户端编号
                var bodyStart = 30
//                MessageParser.setMessageAttribute(tcpMessage, bs)// 设置消息属性
//                if (tcpMessage.getPackageTag()!!)
                // 消息分包处理
//                {
//                    bodyStart = 40
//                    val packageNum = headerBodyAndCode.substring(32, 36)
//                    val packageOrder = headerBodyAndCode.substring(38, 40)
//                    tcpMessage.setPackageNum(Integer.parseInt(packageNum, MessageConst.MESSAGE_RADIX))
//                    tcpMessage.setPackageOrder(Integer.parseInt(packageOrder, MessageConst.MESSAGE_RADIX))
//                    tcpMessage.setMessagePackage(packageNum + packageOrder)
//                }
                val headerBodyAndBodyLength = headerBodyAndCode.length
                val messageBody = headerBodyAndCode.substring(bodyStart, headerBodyAndBodyLength - 2)
                tcpMessage.setMessageBody(messageBody)
                tcpMessage.setMessageCode(messageCode)
                tcpMessage.setOverTag(bs[bsSize - 1])
                MessageParser.tcpMessageDebug(tcpMessage)
            }
            return tcpMessage
        }

        /**
         * 封装一个不加密，不分包的普通消息对象

         * @param messageId   消息编号
         * *
         * @param messageBody 消息体
         * *
         * @return TCPMessage TCPMessage
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun getMessage(messageId: Int, msgType: TCPMsgType, messageBody: String): TCPMessage {
            return getMessage(messageId, 0, msgType, messageBody)
        }

        /**
         * 组装消息对象

         * @param messageId    消息编号
         * *
         * @param deviceId     终端编号
         * *
         * @param messageType  消息类型
         * *
         * @param messageBody  消息体
         * *
         * @param encryptTag   加密标志
         * *
         * @param packageTag   分包标志
         * *
         * @param packageNum   分包数量
         * *
         * @param packageOrder 分包序号
         * *
         * @return TCPMessage TCPMessage
         * *
         * @throws Exception Exception
         */
        @Throws(Exception::class)
        fun getMessage(messageId: Int, mDeviceId: Long, msgType: TCPMsgType, messageBody: String): TCPMessage {
            var deviceId = mDeviceId
            if (messageId <= 0) {
                throw NullPointerException()
            }
            if (deviceId <= 0)
                deviceId = MessageConst.MESSAGE_DEVICE
            var message = TCPMessage()
            message.setStartTag(MessageConst.MESSAGE_FLAG)
            message.setMessageId(messageId)
            message.setMessageLength(messageBody.toByteArray(Charset.forName(Constants.CHARSET_UT)).size)
            message.setDeviceId(deviceId)
            if (msgType == TCPMsgType.DEVICE_MSG) {
                message.setMessageCata("01")
            } else {
                message.setMessageCata("00")
            }
            message.setMessageBody(messageBody.toUpperCase())
            message.setMessageCode(getMessageCode(message))
            message.setOverTag(MessageConst.MESSAGE_FLAG)
            return message
        }

        /**
         * 根据消息对象计算消息校验码

         * @param message TCPMessage
         * *
         * @return byte 消息校验码
         */
        private fun getMessageCode(message: TCPMessage?): Byte {
            if (null == message) {
                throw NullPointerException()
            }
            val hexHeader = getMessageHeader(message)
            val hexBody = message.getMessageBody()
            val stringBuffer = StringBuffer(hexHeader).append(hexBody)
            val bs = ByteUtils.hexStringToBytes(stringBuffer.toString())
            return ByteUtils.bytesXor(bs)
        }

        /**
         * 根据消息对象获取十六进制消息头字符串

         * @param message TCPMessage
         * *
         * @return String 十六进制消息头字符串
         */
        private fun getMessageHeader(message: TCPMessage?): String {
            if (null == message) {
                throw NullPointerException()
            }
            val hexStringBuffer = StringBuffer()
            val hexMessageId = String.format(MessageConst.FOUR_HEX_FORMAT, message.getMessageId()).toUpperCase()
            val hexLength = String.format(MessageConst.FOUR_HEX_FORMAT, message.getMessageLength()).toUpperCase()
            hexStringBuffer.append(hexMessageId).append(message.getMessageCata()).append(hexLength).append(ByteUtils.encodePrimary(message.getDeviceId()))
            return hexStringBuffer.toString().toUpperCase()
        }

//        /**
//         * 获取消息流水号
//
//         * @return Integer 消息流水号
//         */
//        @Synchronized private fun getMessageSerial(): Int? {
//            if (MESSAGE_SERIAL >= MessageConst.MESSAGE_MAXSERIAL)
//                MESSAGE_SERIAL = 1
//            else
//                MESSAGE_SERIAL++
//            return MESSAGE_SERIAL
//        }

//        /**
//         * 计算消息体属性值
//
//         * @param messageBody 消息体
//         * *
//         * @param encryptTag  消息是否加密
//         * *
//         * @param packageTag  消息是否分包
//         * *
//         * @param msgType  消息类型
//         * *
//         * @return TCPMessage TCPMessage
//         * *
//         * @throws Exception Exception
//         */
//        @Throws(Exception::class)
//        private fun setMessageAttribute(message: TCPMessage, msgType: TCPMsgType, messageBody: String): TCPMessage {
//            var messageBody = messageBody
//            messageBody = messageBody.trim { it <= ' ' }
//            val messageLength = messageBody.toByteArray(Charset.forName(Constants.CHARSET_UT)).size
//            var binaryString = Integer.toBinaryString(messageLength)
//            binaryString = String.format("%1$0" + (14 - binaryString.length) + "d", 0) + binaryString// 补位操作，1byte=8bit
////            if (encryptTag) {
////                encrypt = MessageConst.MESSAGE_ENCRYPT
////            }
////            if (packageTag) {
////                packing = MessageConst.MESSAGE_PACKAGE
////            }
//            message.setMessageLength(messageLength)
////            message.setMessageEncrypt(encrypt)
//            val stringBinary = StringBuffer()
//            if (msgType == TCPMsgType.DEVICE_MSG) {
//                stringBinary.append("01")
//            } else {
//                stringBinary.append("00")
//            }
//            message.setMessageCata(stringBinary.toString())
//            stringBinary.append(binaryString)
//            message.setMessageAttribute(Integer.parseInt(stringBinary.toString(), 2))
//            return message
//        }
    }
}