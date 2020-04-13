/**
 * 一个带背景的text
 * 
 * 创建于2020-04-09
 * 定义以下属性
 * title：标题
 * titleStyle：文字样式（text）
 */

import React, {Component} from 'react';
import {View, Text, StyleSheet} from 'react-native';

export default class XXtext extends Component{
    constructor(props) {
        super(props);
    }

    render() {
        return(
            <View style={this.props.style}>
                <Text style={[this.props.titleStyle,{alignSelf:'center'}]}>{this.props.title}</Text>
            </View>
        )
    }
}