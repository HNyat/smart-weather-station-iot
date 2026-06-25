---
name: Deep Space Weather
colors:
  surface: '#131314'
  surface-dim: '#131314'
  surface-bright: '#39393a'
  surface-container-lowest: '#0e0e0f'
  surface-container-low: '#1c1b1d'
  surface-container: '#201f21'
  surface-container-high: '#2a2a2b'
  surface-container-highest: '#353436'
  on-surface: '#e5e2e3'
  on-surface-variant: '#c7c6cd'
  inverse-surface: '#e5e2e3'
  inverse-on-surface: '#313031'
  outline: '#909097'
  outline-variant: '#46464c'
  surface-tint: '#c2c6db'
  primary: '#c2c6db'
  on-primary: '#2b3040'
  primary-container: '#0a0f1e'
  on-primary-container: '#777b8e'
  inverse-primary: '#595e70'
  secondary: '#ddfcff'
  on-secondary: '#00363a'
  secondary-container: '#00f1fe'
  on-secondary-container: '#006a70'
  tertiary: '#dfc1a7'
  on-tertiary: '#3f2d1a'
  tertiary-container: '#1a0c01'
  on-tertiary-container: '#907760'
  error: '#ffb4ab'
  on-error: '#690005'
  error-container: '#93000a'
  on-error-container: '#ffdad6'
  primary-fixed: '#dee1f7'
  primary-fixed-dim: '#c2c6db'
  on-primary-fixed: '#161b2b'
  on-primary-fixed-variant: '#414658'
  secondary-fixed: '#74f5ff'
  secondary-fixed-dim: '#00dbe7'
  on-secondary-fixed: '#002022'
  on-secondary-fixed-variant: '#004f54'
  tertiary-fixed: '#fdddc1'
  tertiary-fixed-dim: '#dfc1a7'
  on-tertiary-fixed: '#281807'
  on-tertiary-fixed-variant: '#58432f'
  background: '#131314'
  on-background: '#e5e2e3'
  surface-variant: '#353436'
  temp-orange: '#FF8C00'
  humidity-cyan: '#00F2FF'
  air-quality-green: '#00FF94'
  warning-red: '#FF3131'
  glass-fill: rgba(255, 255, 255, 0.03)
  glass-border: rgba(255, 255, 255, 0.12)
typography:
  display-lg:
    fontFamily: Sora
    fontSize: 64px
    fontWeight: '600'
    lineHeight: 72px
    letterSpacing: -0.02em
  headline-md:
    fontFamily: Sora
    fontSize: 32px
    fontWeight: '600'
    lineHeight: 40px
  headline-sm:
    fontFamily: Sora
    fontSize: 24px
    fontWeight: '500'
    lineHeight: 32px
  body-lg:
    fontFamily: Inter
    fontSize: 18px
    fontWeight: '400'
    lineHeight: 28px
  body-md:
    fontFamily: Inter
    fontSize: 16px
    fontWeight: '400'
    lineHeight: 24px
  label-caps:
    fontFamily: Inter
    fontSize: 12px
    fontWeight: '700'
    lineHeight: 16px
    letterSpacing: 0.08em
  data-mono:
    fontFamily: Sora
    fontSize: 14px
    fontWeight: '400'
    lineHeight: 20px
rounded:
  sm: 0.25rem
  DEFAULT: 0.5rem
  md: 0.75rem
  lg: 1rem
  xl: 1.5rem
  full: 9999px
spacing:
  grid-margin: 2rem
  bento-gap: 1.5rem
  container-padding: 1.5rem
  element-gap: 0.75rem
---

## Brand & Style

The design system is a futuristic, high-fidelity interface designed for "Hyper-Local Realism." It transforms raw meteorological data into an immersive "Deep Space" experience, targeting tech-enthusiasts and professionals who require instantaneous, glanceable insights.

The aesthetic is a blend of **Glassmorphism** and **Modern Corporate**, utilizing a Bento Box layout to organize complex data streams into clean, modular containers. The interface evokes a sense of being in a high-tech observatory—quiet, precise, and authoritative. By prioritizing high-contrast warnings and vibrant neon accents against a void-like background, the system ensures that critical weather shifts are never missed.

## Colors

The palette is anchored by a "Deep Midnight" base, providing a high-contrast canvas for neon functional colors.

- **Primary Background:** A deep, near-black navy (#0A0F1E) that simulates the atmosphere at night.
- **Data Accents:** Specific hues are reserved for data categories: **Orange** for temperature, **Cyan** for humidity/liquid, and **Green** for air quality/safety.
- **Glassmorphism:** Surfaces are not solid; they use a semi-transparent white fill at 3-5% opacity with a subtle 1px white border at 12% opacity to catch "light" at the edges.
- **Semantic Feedback:** High-saturation **Red** is strictly reserved for critical warnings and sensor failures.

## Typography

This design system utilizes **Sora** for headlines and data displays to leverage its geometric, futuristic character. **Inter** is used for body text and labels to ensure maximum legibility at small sizes.

- **Data Hero:** For primary metrics (like the current temperature), use `display-lg`.
- **Labels:** Secondary information and metadata should use `label-caps` in uppercase to distinguish them from interactive elements.
- **Glanceability:** Ensure high weight (500+) is used for all numerical data points to make them pop against the dark background.

## Layout & Spacing

The layout follows a **Modular Bento Box** grid. This approach breaks the screen into logical rectangular sections that can vary in aspect ratio but maintain a consistent gutter.

- **Grid System:** A 12-column fluid grid for desktop, collapsing to 1 column for mobile.
- **Density:** High. The system should feel data-rich but organized. Use `bento-gap` to ensure that even with glass effects, individual cards feel distinct.
- **Responsive Behavior:** 
  - **Desktop:** Multi-column bento modules.
  - **Tablet:** 2-column stacked modules.
  - **Mobile:** Single column vertical scroll with condensed metric headers.

## Elevation & Depth

Depth is created through transparency and blur rather than traditional shadows.

- **Base Layer:** The Deep Midnight background.
- **Card Layer:** Frosted glass effect using `backdrop-filter: blur(20px)`. This creates a sense of the background "glowing" through the card.
- **Glow Effects:** Critical metrics and active neon elements should have a soft `outer-glow` (drop shadow with 0 offset and high blur) using their respective accent color at 30% opacity.
- **Separation:** Use 1px internal borders to define card edges rather than heavy shadows to keep the interface looking crisp and "lightweight."

## Shapes

The shape language is "Soft-Modern." Large radii are used for the primary bento containers to counteract the clinical nature of the data.

- **Bento Cards:** Use `rounded-xl` (1.5rem) to create a friendly, approachable container.
- **Buttons/Inputs:** Use `rounded-lg` (1rem) for a distinct but harmonious feel.
- **Graphs:** Data points in line charts should be circular, while bar charts should have slightly rounded caps (4px).

## Components

### Glass Cards
The foundation of the Bento grid. Every card must have a `backdrop-filter: blur(20px)`, a 1px `glass-border`, and a `glass-fill`.

### Neon Data Chips
Small indicators for sensor status. They feature a high-contrast background (the accent color) with black text for maximum punch, or a ghost version with an accent-colored border and text.

### Interactive Charts
Use thin lines (2px) for trend data. Implement "Area Glow" where the space beneath a line chart is filled with a 10% opacity gradient of the line's color.

### High-Contrast Warnings
Warnings bypass the glass aesthetic. They should be solid `warning-red` blocks with white text to demand immediate attention.

### Metric Groups
A vertical stack containing a `label-caps` descriptor, a `display-lg` value, and a small trend indicator arrow (Up/Down) placed to the right of the value.