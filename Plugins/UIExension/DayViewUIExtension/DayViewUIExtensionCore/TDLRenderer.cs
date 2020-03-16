using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms.VisualStyles;
using System.Windows.Forms;

using Abstractspoon.Tdl.PluginHelpers;
using Abstractspoon.Tdl.PluginHelpers.ColorUtil;

namespace DayViewUIExtension
{
    class TDLRenderer : Calendar.AbstractRenderer
    {
		private UIExtension.TaskIcon m_TaskIcons;
		private IntPtr m_hWnd;
		private Font m_BaseFont;

		// ------------------------------------------------------------------------

		public TDLRenderer(IntPtr hWnd, UIExtension.TaskIcon taskIcons)
		{
			m_TaskIcons = taskIcons;
			m_hWnd = hWnd;

			ShowParentsAsFolder = false;
            TaskColorIsBackground = false;
            StrikeThruDoneTasks = true;
        }

		public bool ShowParentsAsFolder { get; set; }
		public bool TaskColorIsBackground { get; set; }
        public bool StrikeThruDoneTasks { get; set; }

		public UITheme Theme { get; set; }
		public int TextPadding { get { return 2; } }

        protected override void Dispose(bool mainThread)
        {
            base.Dispose(mainThread);

            if (m_BaseFont != null)
                m_BaseFont.Dispose();
        }

        public override Font BaseFont
        {
            get
            {
                if (m_BaseFont == null)
                {
                    m_BaseFont = new Font("Tahoma", 8, FontStyle.Regular);
                }

                return m_BaseFont;
            }
        }

        public void SetFont(String fontName, int fontSize)
        {
            if ((m_BaseFont.Name == fontName) && (m_BaseFont.Size == fontSize))
                return;

            m_BaseFont = new Font(fontName, fontSize, FontStyle.Regular);
        }
        
        public int GetFontHeight()
        {
            return BaseFont.Height;
        }

        public override System.Drawing.Color HourColor
        {
            get
            {
                return System.Drawing.Color.FromArgb(230, 237, 247);
            }
        }

        public override System.Drawing.Color HalfHourSeperatorColor
        {
            get
            {
                // Halfway between AppLinesDark and window colour
                return InterpolateColors(this.HourSeperatorColor, SystemColors.Window, 0.5f);
            }
        }

        public override System.Drawing.Color HourSeperatorColor
        {
            get
            {
                Color appLineColor = Theme.GetAppDrawingColor(UITheme.AppColor.AppLinesDark);

                if (appLineColor == BackColor)
                    appLineColor = Theme.GetAppDrawingColor(UITheme.AppColor.AppLinesLight);

                return appLineColor;
            }
        }

        public override System.Drawing.Color WorkingHourColor
        {
            get
            {
                return System.Drawing.Color.FromArgb(255, 255, 255);
            }
        }

        public override System.Drawing.Color BackColor
        {
            get
            {
				return Theme.GetAppDrawingColor(UITheme.AppColor.AppBackLight);
            }
        }

        public override System.Drawing.Color SelectionColor
        {
            get
            {
                return System.Drawing.Color.FromArgb(41, 76, 122);
            }
        }

        public System.Drawing.Color TextColor
        {
            get
            {
                return Theme.GetAppDrawingColor(UITheme.AppColor.AppText);
            }
        }

		public void DrawNonWorkingHourRange(Graphics g, Rectangle rect, bool drawBorder, bool hilight)
		{
			if (g == null)
				throw new ArgumentNullException("g");

			using (SolidBrush brush = new SolidBrush(Theme.GetAppDrawingColor(UITheme.AppColor.NonWorkingHours, 128)))
			{
				g.FillRectangle(brush, rect);
			}

			if (drawBorder)
				g.DrawRectangle(SystemPens.WindowFrame, rect);
		}

		public override void DrawHourLabel(System.Drawing.Graphics g, System.Drawing.Rectangle rect, int hour, bool ampm)
        {
            if (g == null)
                throw new ArgumentNullException("g");

            using (SolidBrush brush = new SolidBrush(this.TextColor))
            {
				// Ignore 'ampm' and format for the current regional settings
                string amPmTime = "00";

                if (!String.IsNullOrEmpty(DateTimeFormatInfo.CurrentInfo.AMDesignator))
				{
					if (hour < 12)
						amPmTime = DateTimeFormatInfo.CurrentInfo.AMDesignator;
					else
						amPmTime = DateTimeFormatInfo.CurrentInfo.PMDesignator;

					if (hour != 12)
						hour = hour % 12;
				}

				String hourStr = hour.ToString("##00", System.Globalization.CultureInfo.InvariantCulture);
                
				g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
				g.DrawString(hourStr, HourFont, brush, rect);

                rect.X += ((int)g.MeasureString(hourStr, HourFont).Width + 2);

                g.DrawString(amPmTime, MinuteFont, brush, rect);
				g.TextRenderingHint = TextRenderingHint.SystemDefault;
			}
        }

        public override void DrawMinuteLine(Graphics g, Rectangle rect, bool hour)
        {
            if (g == null)
                throw new ArgumentNullException("g");

            if (hour)
            {
                using (Pen pen = new Pen(Theme.GetAppDrawingColor(UITheme.AppColor.AppLinesDark)))
                    g.DrawLine(pen, rect.Left, rect.Y, rect.Width, rect.Y);
            }
        }

        public override void DrawDayHeader(System.Drawing.Graphics g, System.Drawing.Rectangle rect, DateTime date)
        {
            if (g == null)
                throw new ArgumentNullException("g");

            System.Drawing.Rectangle rHeader = rect;
            rHeader.Height += 2;

            if (date.Date.Equals(DateTime.Now.Date))
            {
                rHeader.Width += 1;

                if (VisualStyleRenderer.IsSupported)
                {
                    var renderer = new VisualStyleRenderer(VisualStyleElement.Header.Item.Hot);
                    renderer.DrawBackground(g, rHeader);
                }
                else
                {
                    using (SolidBrush brush = new SolidBrush(SystemColors.ButtonHighlight))
                        g.FillRectangle(brush, rHeader);
                }
            }
            else
            {
                rHeader.X += 1;

                if (VisualStyleRenderer.IsSupported)
                {
                    var renderer = new VisualStyleRenderer(VisualStyleElement.Header.Item.Normal);
                    renderer.DrawBackground(g, rHeader);
                }
                else
                {
                    using (SolidBrush brush = new SolidBrush(SystemColors.ButtonFace))
                        g.FillRectangle(brush, rHeader);
                }
            }

            using (StringFormat format = new StringFormat())
            {
                format.Alignment = StringAlignment.Center;
                format.FormatFlags = StringFormatFlags.NoWrap;
                format.LineAlignment = StringAlignment.Center;

                using (StringFormat formatdd = new StringFormat())
                {
                    formatdd.Alignment = StringAlignment.Near;
                    formatdd.FormatFlags = StringFormatFlags.NoWrap;
                    formatdd.LineAlignment = StringAlignment.Center;

                    g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

                    // Day of month
                    using (Font fntDayDate = new Font("Tahoma", 9, FontStyle.Bold))
                    {
                        var dateNum = date.ToString(" d");
                        g.DrawString(dateNum, fntDayDate, SystemBrushes.WindowText, rect, formatdd);

                        int strWidth = (int)g.MeasureString(dateNum, fntDayDate).Width;

                        rect.Width -= strWidth;
                        rect.X += strWidth;
                    }

                    // Day of week
                    using (Font fntDay = new Font("Tahoma", 8, FontStyle.Regular))
                    {
                        string dayName = System.Globalization.CultureInfo.CurrentCulture.DateTimeFormat.GetDayName(date.DayOfWeek);
                        int nameWidth = (int)g.MeasureString(dayName, fntDay).Width;

                        // get short day abbr. if narrow dayrect
                        if (rect.Width < nameWidth)
                            dayName = System.Globalization.CultureInfo.CurrentCulture.DateTimeFormat.GetAbbreviatedDayName(date.DayOfWeek);

						TextRenderer.DrawText(g, dayName, fntDay, rect, SystemColors.WindowText);
                        //g.DrawString(dayName, fntDay, SystemBrushes.WindowText, rect, format);
                    }
                }
            }
        }

        public override void DrawDayBackground(System.Drawing.Graphics g, System.Drawing.Rectangle rect)
        {
            //using (SolidBrush backBrush = new SolidBrush(Theme.GetAppDrawingColor(UITheme.AppColor.AppBackDark)))
            //    g.FillRectangle(backBrush, rect);
        }

		public override void DrawHourRange(Graphics g, Rectangle rect, bool drawBorder, bool hilight)
		{
			if (hilight)
			{
				// Draw selection rect
				UIExtension.SelectionRect.Draw(m_hWnd, g, rect.X, rect.Y, rect.Width, rect.Height, true); // transparent
			}
			else
			{
				base.DrawHourRange(g, rect, drawBorder, hilight);
			}
		}

		public bool TaskHasIcon(CalendarItem taskItem)
		{
			return ((m_TaskIcons != null) &&
					(taskItem != null) &&
					(taskItem.HasIcon || (ShowParentsAsFolder && taskItem.IsParent)));
		}

        public override void DrawAppointment(System.Drawing.Graphics g, System.Drawing.Rectangle rect, Calendar.Appointment appointment, bool isSelected, System.Drawing.Rectangle gripRect)
        {
            if (appointment == null)
                throw new ArgumentNullException("appointment");

            if (g == null)
                throw new ArgumentNullException("g");

            if (rect.Width != 0 && rect.Height != 0)
            {
				CalendarItem taskItem = (appointment as CalendarItem);
				bool longAppt = taskItem.IsLongAppt();

				if (!longAppt && (taskItem.StartDate.TimeOfDay.TotalHours == 0.0))
				{
					rect.Y++;
					rect.Height--;
				}

				rect.Width--;

				// Recalculate colours
				Color textColor = taskItem.TaskTextColor;
				Color borderColor = taskItem.TaskTextColor;
				Color fillColor = DrawingColor.SetLuminance(taskItem.TaskTextColor, 0.95f);
				Color barColor = taskItem.TaskTextColor;

				if (taskItem.HasTaskTextColor)
				{
					if (isSelected)
					{
						textColor = DrawingColor.SetLuminance(taskItem.TaskTextColor, 0.3f);
					}
					else if (TaskColorIsBackground && !taskItem.IsDoneOrGoodAsDone)
					{
						textColor = DrawingColor.GetBestTextColor(taskItem.TaskTextColor);
						borderColor = DrawingColor.AdjustLighting(taskItem.TaskTextColor, -0.5f, true);
						barColor = taskItem.TaskTextColor;
						fillColor = taskItem.TaskTextColor;
					}
				}

                using (StringFormat format = new StringFormat())
                {
                    format.Alignment = StringAlignment.Near;
                    format.LineAlignment = (longAppt ? StringAlignment.Center : StringAlignment.Near);

                    // Draw the background of the appointment
					if (isSelected)
					{
                        UIExtension.SelectionRect.Draw(m_hWnd, g, rect.Left, rect.Top, rect.Width, rect.Height, false); // opaque
					}
					else
					{
						using (SolidBrush brush = new SolidBrush(fillColor))
							g.FillRectangle(brush, rect);

						if (taskItem.DrawBorder)
						{
							using (Pen pen = new Pen(borderColor, 1))
								g.DrawRectangle(pen, rect);
						}
					}

					// Draw appointment icon
					bool hasIcon = false;
                    taskItem.IconRect = Rectangle.Empty;

                    if (TaskHasIcon(taskItem))
                    {
						Rectangle rectIcon;
                        int imageSize = DPIScaling.Scale(16);
                        
						if (taskItem.IsLongAppt())
						{
							int yCentre = ((rect.Top + rect.Bottom + 1) / 2);
                            rectIcon = new Rectangle((rect.Left + TextPadding), (yCentre - (imageSize / 2)), imageSize, imageSize);
						}
						else
						{
                            rectIcon = new Rectangle(rect.Left + TextPadding, rect.Top + TextPadding, imageSize, imageSize);
						}

                        if (g.IsVisible(rectIcon) && m_TaskIcons.Get(taskItem.Id))
                        {
							if (longAppt)
							{
								rectIcon.X = (gripRect.Right + TextPadding);
							}
							else
							{
                                gripRect.Y += (imageSize + TextPadding);
                                gripRect.Height -= (imageSize + TextPadding);
							}

							var clipRgn = g.Clip;

							if (rect.Bottom < (rectIcon.Y + imageSize))
								g.Clip = new Region(RectangleF.Intersect(rect, g.ClipBounds));

							m_TaskIcons.Draw(g, rectIcon.X, rectIcon.Y);

							g.Clip = clipRgn;
							
                            hasIcon = true;
                            taskItem.IconRect = rectIcon;

							rect.Width -= (rectIcon.Right - rect.Left);
							rect.X = rectIcon.Right;


						}
                    }

                    // Draw gripper bar
					if (gripRect.Width > 0)
					{
						using (SolidBrush brush = new SolidBrush(barColor))
							g.FillRectangle(brush, gripRect);

						// Draw gripper border
						using (Pen pen = new Pen(DrawingColor.AdjustLighting(barColor, -0.5f, true), 1))
							g.DrawRectangle(pen, gripRect);

						if (!hasIcon)
						{
							rect.X = gripRect.Right;
							rect.Width -= (gripRect.Width + (TextPadding * 2));
						}
					}

					// draw appointment text
					rect.Y += 3;

					if (longAppt)
						rect.Height = m_BaseFont.Height;
					else
						rect.Height -= 3;

					taskItem.TextRect = rect;
                    g.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

                    using (SolidBrush brush = new SolidBrush(textColor))
                    {
                        if (taskItem.IsDone && StrikeThruDoneTasks)
                        {
                            using (Font font = new Font(this.BaseFont, FontStyle.Strikeout))
                            {
                                g.DrawString(appointment.Title, font, brush, rect, format);
                            }
                        }
                        else
                        {
                            g.DrawString(appointment.Title, this.BaseFont, brush, rect, format);
                        }
                    }

                    g.TextRenderingHint = TextRenderingHint.SystemDefault;
                }
            }
        }
    }
}
