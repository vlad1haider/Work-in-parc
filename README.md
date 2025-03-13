<Window x:Class="WpfSketchApp.MainWindow"
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:local="clr-namespace:WpfSketchApp"
        mc:Ignorable="d"
        Title="WPF Sketch App" Height="450" Width="800" Closing="Window_Closing">
    <Grid>
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="Auto"/>
            <ColumnDefinition Width="*"/>
        </Grid.ColumnDefinitions>

        <StackPanel Width="150" Margin="5">
            <Button Content="Новый скетч" Margin="5" Click="NewSketchButton_Click"/>
            <Button Content="Добавить квадрат" Margin="5" Click="AddSquareButton_Click"/>

            <Label Content="Цвет:"/>
            <ComboBox SelectedValue="{Binding SelectedColor}"
                      IsEnabled="{Binding IsFigureSelected}" >
                <ComboBoxItem Content="#4287f5" IsSelected="True" />
                <ComboBoxItem Content="#FF0000" />
                <ComboBoxItem Content="#00FF00" />
                <ComboBoxItem Content="#0000FF" />
                <ComboBoxItem Content="#FFFF00" />
                <ComboBoxItem Content="#FF00FF" />
                <ComboBoxItem Content="#00FFFF" />
            </ComboBox>
            <ListBox x:Name="SketchList" ItemsSource="{Binding Sketches}" DisplayMemberPath="Name" SelectedItem="{Binding CurrentSketch}" SelectionChanged="SketchList_SelectionChanged"/>
        </StackPanel>

        <Canvas x:Name="MainCanvas" Grid.Column="1" Background="LightGray" ClipToBounds="True"
                MouseDown="Canvas_MouseDown" MouseMove="Canvas_MouseMove" MouseUp="Canvas_MouseUp"/>
    </Grid>
</Window>
