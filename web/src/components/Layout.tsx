import { Helmet } from "react-helmet";

export default function Layout({ children, title, home = false }) {
  return (
    <div className="flex flex-col min-h-screen bg-custom_gray_medium text-white">
      <Helmet>
        <title>{home ? `XClicker - ${title}` : `${title} - XClicker`}</title>
      </Helmet>
      
      {children}
    </div>
  );
}
