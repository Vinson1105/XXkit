/**
 * 模态显示，在Modal在基础上增加一层可点击view 
 * 
 * 创建于2020-04-11
 * 1、支持原生的Modal属性设置，除transparent外（transparent写死为true），详见 https://reactnative.cn/docs/modal
 * 2、新增以下封装属性
 *  onBackgroundPress               // 类型：function，背景点击
 *  content                         // 类型：component，用于显示在Modal上
 * 
 * 3、FIXME: 当设置了onBackgroundPress，在content中点击一个view，会触发了这个回调，可能是view的点击事件传递到parent中，需要加以限制！
 * 
 */

import React, {Component} from 'react';
import {View, Modal, TouchableWithoutFeedback, Text} from 'react-native';

export default class XXdialog extends Component{
    static defaultProps = {
        // 以下是原生的Modal的属性
        visible: false,
        supportedOrientations: ['portrait','landscape'],
        onRequestClose: ()=>{},
        onShow: ()=>{},
        animationType: 'slide',

        hardwareAccelerated: false,
        statusBarTranslucent: false,
        onDismiss: ()=>{},
        onOrientationChange: ()=>{},
        presentationStyle: 'overFullScreen',

        // 以下是新增
        onBackgroundPress: ()=>{},
        content:<Text style={{color:'white',textAlign:'center'}}>请设置Content</Text>,
      }

    constructor(props){
        super(props)
    }

    render() {
        return (
            <Modal 
                    visible={this.props.visible} 
                    supportedOrientations={this.props.supportedOrientations}
                    onRequestClose={this.props.onRequestClose}
                    onShow={this.props.onShow}
                    transparent={true}

                    animationType={this.props.animationType}
                    hardwareAccelerated={this.props.hardwareAccelerated}
                    statusBarTranslucent={this.props.statusBarTranslucent}
                    onDismiss={this.props.onDismiss}
                    onOrientationChange={this.props.onOrientationChange}

                    presentationStyle={this.props.presentationStyle}
                >
                    <TouchableWithoutFeedback onPress={this.props.onBackgroundPress}>
                        <View style={[{width:'100%',height:'100%',alignSelf:'center',justifyContent:'center',backgroundColor:'#0000007f'}, this.props.style]}>
                            {this.props.content}
                        </View>
                    </TouchableWithoutFeedback>
                </Modal>
        );
    }
}