/**
 * 一个带背景的button
 * 
 * 创建于2020-04-08
 * 定义以下属性
 * type：[title、image]
 * title：button的标题
 * titleStyle：文字样式（text）
 * source：button的图片
 * onPress：点击事件
 */

import React, {Component} from 'react';
import PropTypes from 'prop-types';
import {View, TouchableWithoutFeedback, Text, StyleSheet, Image} from 'react-native';

export default class XXbutton extends Component {
  constructor(props) {
    super(props);
  }

  static propTypes = {
    type: PropTypes.oneOf(['title', 'image', 'detail']),
  }
  static defaultProps = {
    type: 'title',
  }

  render() {
    if(this.props.type === 'title'){
      return (
        <View style={[{flexDirection:'column', justifyContent:'center'}, this.props.style]}>
          <TouchableWithoutFeedback onPress={this.props.onPress} disabled={this.props.disabled}>
            <View style={{width:'100%',height:'100%',justifyContent:'center'}}>
              <Text style={[{alignSelf:'center',textAlign:'center'}, this.props.titleStyle]}>{this.props.title}</Text>
            </View>
          </TouchableWithoutFeedback>
        </View>
      );
    }
    else if(this.props.type === 'image'){
      return (
        <View style={[{flexDirection:'column', justifyContent:'center', alignItems:'center'}, this.props.style]}>
          <TouchableWithoutFeedback onPress={this.props.onPress} disabled={this.props.disabled}>
            <Image source={this.props.source} style={{width:'100%',height:'100%',alignSelf:'center'}} />
          </TouchableWithoutFeedback>
        </View>
      );
    }
    else if(this.props.type === 'detail'){
      return null;
    }
    else{
      return null;
    }
  }
}
